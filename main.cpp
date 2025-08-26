#include "headers/window.hpp"
#include "headers/renderpipeline.hpp"

int main() {
    Window Game("Game", 800, 600, API::OPENGL);


    if (Game.init() != 0) return -1;

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }
        Game.SetClearColor(0.6f, 0.8f, 1.0f, 1.0f);
        Game.clear();
        Game.update();
    }

    return 0;
}