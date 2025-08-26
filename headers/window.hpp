#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_opengl.h>


enum class API {
    OPENGL,
    VULKAN,
    DX11,
    DX12,
    METAL
};

class Window {
public:
    const char* name;
    int Width;
    int Height;
    API GraphicsAPI;

    SDL_Window* sdlWindow = nullptr;
    SDL_GLContext glContext = nullptr;

    Window(const char* name, int Width, int Height, API GraphicsAPI);
    ~Window();

    int init();

    void clear();

    void SetClearColor(float r, float g, float b, float alpha);

    void update();
};
