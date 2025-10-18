#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include "includes/6502.h"
#include "includes/bus.h"
#include "includes/ppu.h"
#include "includes/cartridge.h"
#include "includes/apu.h"
// #include "vendored/tinyfiledialogs/tinyfiledialogs.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <math.h>
using namespace std;

const float PI = acos(-1.);
uint32_t frame[256 * 240]; // frontend-owned buffer
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

SDL_AudioDeviceID device_id;
SDL_AudioSpec spec;
SDL_Texture *texture = NULL;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

unsigned char buttons = 0xFF;
int wait = 0;
double sampleRate = 44100.0;
bool loaded = false;

void audio_callback(void *userdata, Uint8 *stream, int length)
{
    wait = 1;
    apu.getSampling((Uint16 *)stream, length / sizeof(Sint16), sampleRate);
    wait = 0;
}

void main_loop()
{
    SDL_Event e;
    // Do event loop
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_KEYUP:
            switch (e.key.keysym.sym)
            {
            case SDLK_RIGHT:
                buttons |= 0x80;
                break; // Right
            case SDLK_LEFT:
                buttons |= 0x40;
                break; // Left
            case SDLK_DOWN:
                buttons |= 0x20;
                break; // Down
            case SDLK_UP:
                buttons |= 0x10;
                break; // Up
            case SDLK_RETURN:
                buttons |= 0x08;
                break; // Start
            case SDLK_RSHIFT:
                buttons |= 0x04;
                break; // Select
            case SDLK_z:
                buttons |= 0x02;
                break; // B
            case SDLK_x:
                buttons |= 0x01;
                break; // A
            }
            break;
        case SDL_KEYDOWN:
            switch (e.key.keysym.sym)
            {
            case SDLK_RIGHT:
                buttons &= ~0x80;
                break; // Right
            case SDLK_LEFT:
                buttons &= ~0x40;
                break; // Left
            case SDLK_DOWN:
                buttons &= ~0x20;
                break; // Down
            case SDLK_UP:
                buttons &= ~0x10;
                break; // Up
            case SDLK_RETURN:
                buttons &= ~0x08;
                break; // Start
            case SDLK_RSHIFT:
                buttons &= ~0x04;
                break; // Select
            case SDLK_z:
                buttons &= ~0x02;
                break;
            case SDLK_x:
                buttons &= ~0x01;
                break; // A
            }
            break;
        }
    }
    bus.button1 = buttons;
    bus.button2 = 0xFF;
    // Do physics loop
    if (loaded)
    {
        long long int cyc = cpu.total_cycles;
        while (!ppu.okVblank)
        {
            while(wait);
            cpu.nextInstruction();
        }
        ppu.okVblank = false;
        wait = false;
        // Do rendering loop
        for (int y = 0; y < 240; ++y)
        {
            for (int x = 0; x < 256; ++x)
            {
                Color pixel = ppu.framebuffer[x][y];
                // pack into 0xRRGGBB (32-bit)
                frame[y * 256 + x] = (pixel.R << 16) | (pixel.G << 8) | (pixel.B);
            }
        }
    }
    // renderer
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_UpdateTexture(texture, NULL, frame, 256 * sizeof(uint32_t));
    SDL_Rect dstRect = {0, 0, 512, 480};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_RenderPresent(renderer);
}

// Called from JS to resume the loop
extern "C" void load_cartridge()
{
    bool nloaded = cartridge.read("/file.nes");
    SDL_PauseAudioDevice(device_id, 1);
    if (nloaded)
    {
        loaded = true;
        ppu.powerON();
        cpu.powerON();
        cpu.reset();
        apu.reset();
        SDL_PauseAudioDevice(device_id, 0);
    }else if(loaded){
        SDL_PauseAudioDevice(device_id, 0);
    }
    
}

int main()
{
    printf("start\n");
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    bus.connectAPU(&apu);
    bus.connectCPU(&cpu);
    bus.connectCartridge(&cartridge);
    bus.connectPPU(&ppu);

    spec.freq = (int)sampleRate;    // Sample rate (Hz)
    spec.format = AUDIO_U16SYS;     // 16-bit signed samples
    spec.channels = 1;              // Mono sound
    spec.samples = 512;             // Buffer size (number of samples per callback)
    spec.callback = audio_callback; // Callback function
    device_id = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);

    window = SDL_CreateWindow("NES Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 480, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
    emscripten_set_main_loop(main_loop, 60, 1);
    SDL_PauseAudioDevice(device_id, 0);
}