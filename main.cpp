#include "headers/window.hpp"
#include "headers/renderpipeline.hpp"

int main() {
    Window Game("Game", 800, 600, API::OPENGL);
    pipeline renderer(&Game, 90, 0.1f, 100.0f);

    renderer.init("shaders/vert.glsl", "shaders/frag.glsl", Game);

    TextureSuiteRC CubeTextures = {
        .AlbedoPath="image.png",
        .NormalPath="image.png",
        .SpecularPath="image.png",
        .MetallicPath="image.png",
        .EmissionPath="image.png",
    };

    WorldObject Cube = WorldObject({
        .Mesh=Game.LoadOBJ("cube.obj", CubeTextures),
        .Transform=Vector3({0.0f, 0.0f, 0.0f}),
        .Rotation=Vector3({0.0f, 0.0f, 0.0f}),
        .Scale=Vector3({1.0f, 1.0f, 1.0f})
    });

    renderer.WorldObjects.push_back(Cube);

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }
        renderer.GameWindow->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        renderer.render();
    }

    return 0;
}