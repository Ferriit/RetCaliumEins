#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <SDL2/SDL_opengl.h>


enum class API {
    OPENGL,
    VULKAN,
    DX11,
    DX12,
    METAL
};

struct Mesh {
    GLuint VAO, VBO;
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

    Mesh UploadMesh(float vertices[], size_t vertexCount);
    
    GLuint UploadTexture(const char* path);
};
