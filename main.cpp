#include "headers/window.hpp"
#include "headers/renderpipeline.hpp"

int main() {
    Window Game("Game", 800, 600, API::OPENGL);
    pipeline renderer(&Game, 90, 0.1f, 100.0f);

    renderer.init("shaders/vert.glsl", "shaders/frag.glsl", Game);


    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }
        renderer.GameWindow->SetClearColor(0.8f, 0.9f, 1.0f, 1.0f);
        renderer.render();
    }

    return 0;
}