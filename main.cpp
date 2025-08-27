#include "headers/window.hpp"
#include "headers/renderpipeline.hpp"

int main() {
    Window Game("Game", 800, 600, API::OPENGL);
    pipeline renderer(&Game, 90, 0.1f, 100.0f);

    renderer.init("shaders/vert.glsl", "shaders/albedo_only_frag.glsl", Game);

    TextureSuiteRC CubeTextures = {
        .AlbedoPath="image.png",
        .NormalPath="image.png",
        .SpecularPath="image.png",
        .MetallicPath="image.png",
        .EmissionPath="image.png",
    };

    float angle;

    WorldObject* Cube = new WorldObject[1] {{
        .Mesh=Game.LoadOBJ("cube.obj", CubeTextures),
        .Transform=Vector3({0.0f, 0.0f, 0.0f}),
        .Rotation=Vector3({0.0f, 0.0f, 0.0f}),
        .Scale=Vector3({1.0f, 1.0f, 1.0f})
    }};

    renderer.WorldObjects.push_back(Cube);

    renderer.GameWindow->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        Cube->Rotation.y = angle;

        angle += 0.5f;

        renderer.render();
    }

    delete[] Cube;

    return 0;
}