#include "headers/window.hpp"
#include <SDL2/SDL_opengl.h>


Window::Window(const char* name, int Width, int Height, API GraphicsAPI)
: name(name), Width(Width), Height(Height), GraphicsAPI(GraphicsAPI) { }

Window::~Window() {
    if (GraphicsAPI == API::OPENGL && glContext) {
        SDL_GL_DeleteContext(glContext);
    }
    if (sdlWindow) SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
}


int Window::init() {
    Uint32 GPUFlag;
    switch (this->GraphicsAPI) {
        case API::OPENGL:
            GPUFlag = SDL_WINDOW_OPENGL;
            break;

        case API::VULKAN:
            GPUFlag = SDL_WINDOW_VULKAN;
            break;

        default:
            GPUFlag = SDL_WINDOW_SHOWN;
            break;
    };

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    if (this->GraphicsAPI == API::OPENGL) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    }

    sdlWindow = SDL_CreateWindow(
        this->name,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        this->Width, this->Height,
        GPUFlag
    );

    if (this->GraphicsAPI == API::OPENGL) {
        glContext = SDL_GL_CreateContext(sdlWindow);
        if (!glContext) {
                std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << "\n";
                return -1;
            }
    }

    return 0;
}

void Window::clear() {
    if (this->GraphicsAPI == API::OPENGL) {
        glClear(GL_COLOR_BUFFER_BIT);
    }
}


void Window::SetClearColor(float r, float g, float b, float alpha) {
    glClearColor(r, g, b, alpha);
}


void Window::update() {
    if (this->GraphicsAPI == API::OPENGL) {
        SDL_GL_SwapWindow(this->sdlWindow);
    }
}