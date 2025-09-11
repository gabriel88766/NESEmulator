#include <SDL3/SDL.h>
#include <iostream>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "emuNES",                // window title
        640,                     // width
        480,                     // height
        SDL_WINDOW_RESIZABLE         // flags
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    // Keep window open for 5 seconds
    SDL_Delay(5000);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}