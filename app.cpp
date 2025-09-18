#include "includes/6502.h"
#include "includes/bus.h"
#include "includes/ppu.h"
#include "includes/cartridge.h"
#include "includes/apu.h"
#include "vendored/tinyfiledialogs/tinyfiledialogs.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <math.h>
using namespace std;

const float PI = acos(-1.);
uint32_t frame[256 * 240];  // frontend-owned buffer
long long int nvb = 0;
const long long int clock_frame = 29780;
const long long int clock_vblank = 27314;
long long int ntk = 7457;
const long long int clock_apu = 7457;

static Bus bus;
static CPU cpu;
static Cartridge cartridge;
static PPU ppu;
static APU apu;

bool wait = false;
double sampleRate = 44100.0;

void audio_callback(void* userdata, Uint8* stream, int length) {
    apu.getSampling((Sint16*) stream, length / sizeof(Sint16), sampleRate);
}



int main(int argc, char** args){
    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
		cout << "Error initializing SDL: " << SDL_GetError() << endl;
		return 1;
	} 

    SDL_AudioSpec spec;

    spec.freq = (int)sampleRate;                // Sample rate (Hz)
    spec.format = AUDIO_S16SYS;        // 16-bit signed samples
    spec.channels = 1;                 // Mono sound
    spec.samples = 512;               // Buffer size (number of samples per callback)
    spec.callback = audio_callback;    // Callback function
    SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    
    
    SDL_PauseAudioDevice(device_id, 0); // start audio


    SDL_Texture* texture = NULL;
	SDL_Window* window = NULL;

    window = SDL_CreateWindow( "EmuNES - Click the blue button to choose the ROM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 504, SDL_WINDOW_SHOWN );

    if ( !window ) {
		cout << "Error creating window: " << SDL_GetError()  << endl;
		return 1;
	}

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 256, 240);

    SDL_PauseAudio(0);

    

    
    bus.connectAPU(&apu);
    bus.connectCPU(&cpu);
    bus.connectCartridge(&cartridge);
    bus.connectPPU(&ppu);


    bool running = true;
    unsigned char buttons = 0xFF;
    int X = 0;
    bool loaded = false;
    while (running) {
        SDL_Rect button = {0, 0, 70, 25};
        Uint64 start = SDL_GetPerformanceCounter();
        SDL_Event e;
        // Do event loop
        while (SDL_PollEvent(&e)) {
            switch(e.type){
                case SDL_QUIT:
                    return false;
                    break;
                case SDL_KEYUP:
                    switch (e.key.keysym.sym) {
                        case SDLK_RIGHT:  buttons |= 0x80; break; // Right
                        case SDLK_LEFT:   buttons |= 0x40; break; // Left
                        case SDLK_DOWN:   buttons |= 0x20; break; // Down
                        case SDLK_UP:     buttons |= 0x10; break; // Up
                        case SDLK_RETURN: buttons |= 0x08; break; // Start
                        case SDLK_RSHIFT: buttons |= 0x04; break; // Select
                        case SDLK_z:      buttons |= 0x02; break; // B
                        case SDLK_x:      buttons |= 0x01; break; // A
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_RIGHT:  buttons &= ~0x80; break; // Right
                        case SDLK_LEFT:   buttons &= ~0x40; break; // Left
                        case SDLK_DOWN:   buttons &= ~0x20; break; // Down
                        case SDLK_UP:     buttons &= ~0x10; break; // Up
                        case SDLK_RETURN: buttons &= ~0x08; break; // Start
                        case SDLK_RSHIFT: buttons &= ~0x04; break; // Select 
                        case SDLK_z:      buttons &= ~0x02; break; // B
                        case SDLK_x:      buttons &= ~0x01; break; // A
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(e.button.button == SDL_BUTTON_LEFT){
                        if (e.button.x >= button.x && e.button.x < button.x + button.w &&
                            e.button.y >= button.y && e.button.y < button.y + button.h) {
                            // Button clicked!
                            ppu.printNametables();
                            SDL_PauseAudioDevice(device_id, 1);
                            const char *format[] = {"*.nes"};
                            auto filename = tinyfd_openFileDialog("Select Nes File", NULL, 1, format, "Nes file(.nes)", 0);
                            loaded = cartridge.read(filename);
                            if(!loaded) break;
                            ppu.powerON();
                            cpu.powerON();
                            cpu.reset();
                            apu.reset();
                            SDL_PauseAudioDevice(device_id, 0);
                        }
                    }
                    break;
            }
            if (e.type == SDL_QUIT) running = false;
        }
        bus.button1 = buttons;
        bus.button2 = 0xFF;
        // Do physics loop
        if(loaded){
            while(!ppu.okVblank){
                cpu.nextInstruction();
            }
            ppu.okVblank = false;
            wait = false;
            // Do rendering loop
            for (int y = 0; y < 240; ++y) {
                for (int x = 0; x < 256; ++x) {
                    Color pixel = ppu.framebuffer[x][y];
                    // pack into 0xRRGGBB (32-bit)
                    frame[y * 256 + x] = (pixel.R << 16) | (pixel.G << 8) | (pixel.B);
                }
            }
        }
        //renderer
        SDL_RenderClear(renderer);

        SDL_Rect menubar = {0, 0, 512, 25};
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderFillRect(renderer, &menubar);

        SDL_UpdateTexture(texture, NULL, frame, 256 * sizeof(uint32_t));
        
        SDL_Rect dstRect = {0, 25, 512, 480};
        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        

        
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        int hover = (mx >= button.x && mx < button.x + button.w &&
                    my >= button.y && my < button.y + button.h);
        if (hover) SDL_SetRenderDrawColor(renderer, 120, 120, 255, 255);
        else       SDL_SetRenderDrawColor(renderer, 50, 50, 255, 255);
        SDL_RenderFillRect(renderer, &button);


        SDL_RenderPresent(renderer);

        Uint64 end = SDL_GetPerformanceCounter();

        float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
        cout << 16.666f - elapsedMS << "\n";
        // Cap to 60 FPS
        if(16.666f - elapsedMS > 0) SDL_Delay(floor(16.666f - elapsedMS));
    }

    SDL_CloseAudio();
	SDL_DestroyWindow( window );
	SDL_Quit();
    return 0;
}