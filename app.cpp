#include "includes/6502.h"
#include "includes/bus.h"
#include "includes/ppu.h"
#include "includes/cartridge.h"
#include "includes/apu.h"
#include "vendored/tinyfiledialogs/tinyfiledialogs.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <math.h>
using namespace std;

const float PI = acos(-1.);

static Bus bus;
static CPU cpu;
static Cartridge cartridge;
static PPU ppu;
static APU apu;

double sampleRate = 44100.0;

Uint64 timestamp_start;
float speed = 1;

SDL_AudioStream *stream = NULL;

void resyncaudio(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount){
    Uint64 rsize = SDL_GetAudioStreamAvailable(stream) / sizeof(float);
    if(rsize < 2000){
        const int need = 1'789'773 / 25; //40ms of audio
        std::vector<float> batch(need, 0);
        SDL_PutAudioStreamData(stream, batch.data(), batch.size() * sizeof(float));
        // speed -= 0.0001;
        cerr << "under" << endl;
    }
    SDL_SetAudioStreamFrequencyRatio(stream, speed);

}


int main(int argc, char** args){
    
    freopen("logs", "w", stdout);
    // freopen("logs", "r", stdin);
    if (!SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		cout << "Error initializuming SDL: " << SDL_GetError() << endl;
		return 1;
	} 

    SDL_AudioSpec src, dst;
    
    src.freq = 1789773;
    src.channels = 1;
    src.format = SDL_AUDIO_F32;
    dst.freq = 44100;
    dst.channels = 1;
    dst.format = SDL_AUDIO_F32;


    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &dst, NULL, NULL);
    if(!stream){
        cout << "Error initializing stream: " << SDL_GetError() << endl;
        return 1;
    }
   
    SDL_SetAudioStreamFormat(stream, &src, &dst);
     SDL_SetAudioStreamGetCallback(stream, &resyncaudio, NULL);
    SDL_PauseAudioDevice(SDL_GetAudioStreamDevice(stream));


    SDL_Texture* texture = NULL;
	SDL_Window* window = NULL;

    window = SDL_CreateWindow( "EmuNES - Click the blue button to choose the ROM", 512, 504, SDL_EVENT_WINDOW_SHOWN );

    if ( !window ) {
		cout << "Error creating window: " << SDL_GetError()  << endl;
		return 1;
	}

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL); //SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    // SDL_SetRenderVSync(renderer, -1);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, 256, 240);

    Uint64 timestamp = SDL_GetTicksNS();
    
    memset(ppu.framebuffer, 0xFF, sizeof(ppu.framebuffer));
    
    bus.connectAPU(&apu);
    bus.connectCPU(&cpu);
    bus.connectCartridge(&cartridge);
    bus.connectPPU(&ppu);


    bool running = true;
    unsigned char buttons = 0xFF;
    int X = 0;
    bool loaded = false;
    bool inited = false;
    
    int frames = 0;
    while (running) {
        SDL_FRect button = {0, 0, 70, 25};
        SDL_Event e;
        // Do event loop
        while (SDL_PollEvent(&e)) {
            switch(e.type){
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_KEY_UP:
                    switch (e.key.key) {
                        case SDLK_RIGHT:  buttons |= 0x80; break; // Right
                        case SDLK_LEFT:   buttons |= 0x40; break; // Left
                        case SDLK_DOWN:   buttons |= 0x20; break; // Down
                        case SDLK_UP:     buttons |= 0x10; break; // Up
                        case SDLK_RETURN: buttons |= 0x08; break; // Start
                        case SDLK_RSHIFT: buttons |= 0x04; break; // Select
                        case SDLK_Z:      buttons |= 0x02; break; // B
                        case SDLK_X:      buttons |= 0x01; break; // A
                    }
                    break;
                case SDL_EVENT_KEY_DOWN:
                    switch (e.key.key) {
                        case SDLK_RIGHT:  buttons &= ~0x80; break; // Right
                        case SDLK_LEFT:   buttons &= ~0x40; break; // Left
                        case SDLK_DOWN:   buttons &= ~0x20; break; // Down
                        case SDLK_UP:     buttons &= ~0x10; break; // Up
                        case SDLK_RETURN: buttons &= ~0x08; break; // Start
                        case SDLK_RSHIFT: buttons &= ~0x04; break; // Select 
                        case SDLK_Z:      buttons &= ~0x02; break; // B
                        case SDLK_X:      buttons &= ~0x01; break; // A
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if(e.button.button == SDL_BUTTON_LEFT){
                        if (e.button.x >= button.x && e.button.x < button.x + button.w &&
                            e.button.y >= button.y && e.button.y < button.y + button.h) {
                            // Button clicked!
                            SDL_PauseAudioDevice(SDL_GetAudioStreamDevice(stream));
                            const char *format[] = {"*.nes"};
                            auto filename = tinyfd_openFileDialog("Select Nes File", NULL, 1, format, "Nes file(.nes)", 0);
                            if (!filename) { SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream)); break; }
                            loaded = cartridge.read(filename);
                            if(!loaded) break;
                            frames = 0;
                            timestamp_start = SDL_GetTicksNS();
                            SDL_ClearAudioStream(stream);
                            const int need = 1'789'773 / 25; //40ms of audio
                            std::vector<float> batch(need, 0);
                            SDL_PutAudioStreamData(stream, batch.data(), batch.size() * sizeof(float));
                            ppu.powerON();
                            cpu.powerON();
                            cpu.reset();
                            apu.reset();
                            SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));
                        }
                    }
                    break;
            }
        }
        bus.button1 = buttons;
        bus.button2 = 0xFF;
        // Do physics loop
        Uint64 rsize = SDL_GetAudioStreamAvailable(stream) / sizeof(float);
        if(loaded && rsize < 20000){
            long long int cyc = cpu.total_cycles;
            while(!ppu.okVblank){
                cpu.nextInstruction();
            }
            std::vector<float> batch(apu.samples.begin(), apu.samples.end());
            SDL_PutAudioStreamData(stream, batch.data(), batch.size() * sizeof(float));
            apu.samples.clear();
            ppu.okVblank = false;
        }else if(loaded && rsize >= 20000){
            // speed += 0.0001;
            cerr << "over" << endl;
            SDL_Delay(50);
        }
        //renderer
        SDL_RenderClear(renderer);
        SDL_FRect menubar = {0, 0, 512, 25};
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderFillRect(renderer, &menubar);
        SDL_UpdateTexture(texture, NULL, ppu.framebuffer, 256 * sizeof(uint32_t));
        
        SDL_FRect dstRect = {0, 25, 512, 480};
        
        

        
        float mx, my;
        SDL_GetMouseState(&mx, &my);
        int hover = (mx >= button.x && mx < button.x + button.w &&
                    my >= button.y && my < button.y + button.h);
        if (hover) SDL_SetRenderDrawColor(renderer, 120, 120, 255, 255);
        else       SDL_SetRenderDrawColor(renderer, 50, 50, 255, 255);
        SDL_RenderFillRect(renderer, &button);
        SDL_RenderTexture(renderer, texture, NULL, &dstRect);


        SDL_RenderPresent(renderer);
        
        const Uint64 X = 16'639'267LL;
        Uint64 timestamp_now = SDL_GetTicksNS();
        Uint64 elapsedMS = (timestamp_now - timestamp_start - X * frames);
        frames += 1;
        
        if(elapsedMS <= X) SDL_DelayNS(X - elapsedMS);
        else timestamp_start = timestamp_now - frames * X;
    }

    SDL_DestroyAudioStream(stream);
	SDL_DestroyWindow( window );
	SDL_Quit();
    return 0;
}
