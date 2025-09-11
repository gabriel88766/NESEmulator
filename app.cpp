#include "includes/6502.h"
#include "includes/bus.h"
#include "includes/ppu.h"
#include "includes/cartridge.h"
#include <SDL2/SDL.h>
#include <iostream>
using namespace std;


long long int nvb = 0;
const long long int clock_frame = 29829;


void SetPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    if (x < 0 || y < 0 || x >= surface->w || y >= surface->h) return;

    int bpp = surface->format->BytesPerPixel;
    Uint8* pixel_ptr = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            *pixel_ptr = color;
            break;
        case 2:
            *(Uint16*)pixel_ptr = color;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                pixel_ptr[0] = (color >> 16) & 0xFF;
                pixel_ptr[1] = (color >> 8) & 0xFF;
                pixel_ptr[2] = color & 0xFF;
            } else {
                pixel_ptr[0] = color & 0xFF;
                pixel_ptr[1] = (color >> 8) & 0xFF;
                pixel_ptr[2] = (color >> 16) & 0xFF;
            }
            break;
        case 4:
            *(Uint32*)pixel_ptr = color;
            break;
    }
}

int main(int argc, char** args){
    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK ) < 0) {
		cout << "Error initializing SDL: " << SDL_GetError() << endl;
		return 1;
	} 
    SDL_Surface* winSurface = NULL;
	SDL_Window* window = NULL;

    window = SDL_CreateWindow( "Nes Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 480, SDL_WINDOW_SHOWN );

    if ( !window ) {
		cout << "Error creating window: " << SDL_GetError()  << endl;
		return 1;
	}

    winSurface = SDL_GetWindowSurface( window );

	if ( !winSurface ) {
		cout << "Error getting surface: " << SDL_GetError() << endl;
		return 1;
	}

    Bus bus;
    CPU cpu;
    Cartridge cartridge;
    PPU ppu;
    bus.connectCPU(&cpu);
    bus.connectCartridge(&cartridge);
    bus.connectPPU(&ppu);
    cartridge.read("testROM/FieldCombat.nes"); //this must be selectable with a button
    cpu.powerON();
    bool running = true;
    SDL_FillRect( winSurface, NULL, SDL_MapRGB( winSurface->format, 255, 0, 255 ) );
    SDL_UpdateWindowSurface( window );
    
    while (running) {
        Uint64 start = SDL_GetPerformanceCounter();
        SDL_Event e;
        // Do event loop
        unsigned char buttons = 0x00;
        while (SDL_PollEvent(&e)) {
            switch(e.type){
                case SDL_QUIT:
                    return false;
                    break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_z:      buttons |= 0x02; break; // B
                        case SDLK_x:      buttons |= 0x01; break; // A
                        case SDLK_RSHIFT: buttons |= 0x04; break; // Select
                        case SDLK_RETURN: buttons |= 0x08; break; // Start
                        case SDLK_UP:     buttons |= 0x10; break; // Up
                        case SDLK_DOWN:   buttons |= 0x20; break; // Down
                        case SDLK_LEFT:   buttons |= 0x40; break; // Left
                        case SDLK_RIGHT:  buttons |= 0x80; break; // Right
                    }
                    break;
            }


            if (e.type == SDL_QUIT) running = false;

        }
        // Do physics loop
        while(cpu.total_cycles < nvb){
            cpu.nextInstruction();
        }
        nvb += clock_frame;
        ppu.vblank();
        // Do rendering loop
        if (SDL_MUSTLOCK(winSurface)) {
            SDL_LockSurface(winSurface);
        }
        for (int x = 0; x < 256; ++x) {
            for (int y = 0; y < 240; ++y) {
                Color pixel = ppu.framebuffer[x][y];
                Uint32 sdl_color = SDL_MapRGB(winSurface->format, pixel.R, pixel.G, pixel.B);
                SetPixel(winSurface, 2*x, 2*y, sdl_color);
                SetPixel(winSurface, 2*x+1, 2*y, sdl_color);
                SetPixel(winSurface, 2*x, 2*y+1, sdl_color);
                SetPixel(winSurface, 2*x+1, 2*y+1, sdl_color);
            }
        }
        if (SDL_MUSTLOCK(winSurface)) {
            SDL_UnlockSurface(winSurface);
        }
        SDL_UpdateWindowSurface(window);


        Uint64 end = SDL_GetPerformanceCounter();

        float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

        // Cap to 60 FPS
        SDL_Delay(floor(16.666f - elapsedMS));

    }

	SDL_DestroyWindow( window );
	SDL_Quit();
    return 0;
}