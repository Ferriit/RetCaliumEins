#include "headers/window.hpp"

int main() {
    Window Game("Game", 800, 600, API::OPENGL);

    if (Game.init() != 0) return -1;

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }
    }

    return 0;
}