#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include "includes/6502.h"
#include "includes/bus.h"
#include "includes/ppu.h"
#include "includes/cartridge.h"
#include "includes/apu.h"
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

SDL_AudioDeviceID device_id;
SDL_AudioSpec spec, have;
SDL_Texture *texture = NULL;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

Uint64 timestamp_start;
float speed = 1;

unsigned char buttons = 0xFF;
double sampleRate = 44100.0;
bool loaded = false;
SDL_AudioStream *stream = NULL;
Uint64 frames;

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

void game_loop(){
        SDL_Event e;
        // Do event loop
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
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
            }
        }
        bus.button1 = buttons;
        bus.button2 = 0xFF;
        // Do physics loop
        
        const Uint64 X = 16'639'267LL;
        Uint64 timestamp_now = SDL_GetTicksNS();

        while(loaded && frames * X + timestamp_start <= timestamp_now){
            frames++;
            while(!ppu.okVblank){
                cpu.nextInstruction();
            }
            std::vector<float> batch(apu.samples.begin(), apu.samples.end());
            SDL_PutAudioStreamData(stream, batch.data(), batch.size() * sizeof(float));
            apu.samples.clear();
            ppu.okVblank = false;
        }

        Uint64 rsize = SDL_GetAudioStreamAvailable(stream) / sizeof(float);
        if(loaded && rsize >= 20000){
            // speed += 0.0001;
            cerr << "over" << endl;
            timestamp_start += 50'000'000; //delay 50ms
        }
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_UpdateTexture(texture, NULL, ppu.framebuffer, 256 * sizeof(uint32_t));
        SDL_FRect dstRect = {0, 0, 512, 480};
        SDL_RenderTexture(renderer, texture, NULL, &dstRect);
        SDL_RenderPresent(renderer);
        //Timer
        
    
}


// Called from JS to resume the loop
extern "C" void load_cartridge(){
    bool nloaded = cartridge.read("/file.nes");
    SDL_PauseAudioDevice(device_id);
    if (nloaded)
    {
        loaded = true;
        ppu.powerON();
        cpu.powerON();
        cpu.reset();
        apu.reset();
        SDL_ClearAudioStream(stream);
        const int need = 1'789'773 / 25; //40ms of audio
        std::vector<float> batch(need, 0);
        frames = 0;
        timestamp_start = SDL_GetTicksNS();
        SDL_PutAudioStreamData(stream, batch.data(), batch.size() * sizeof(float));
        SDL_ResumeAudioDevice(device_id);
    }else if(loaded){
        SDL_ResumeAudioDevice(device_id);
    }
    
}

int main()
{
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    bus.connectAPU(&apu);
    bus.connectCPU(&cpu);
    bus.connectCartridge(&cartridge);
    bus.connectPPU(&ppu);

    SDL_AudioSpec src, dst;
    
    src.freq = 1789773;
    src.channels = 1;
    src.format = SDL_AUDIO_F32;
    dst.freq = 44100;
    dst.channels = 1;
    dst.format = SDL_AUDIO_F32;

    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &dst, NULL, NULL);
    SDL_SetAudioStreamFormat(stream, &src, &dst);
    
    device_id = SDL_GetAudioStreamDevice(stream);

    window = SDL_CreateWindow("NES Emulator", 512, 480, SDL_EVENT_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, NULL);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, 256, 240);
    timestamp_start = SDL_GetTicksNS();
    frames = 0;
    SDL_PauseAudioDevice(device_id);
    emscripten_set_main_loop(game_loop, 0, 1);
    
    
}