#include "headers/window.hpp"


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

        int w, h;
        SDL_GetWindowSize(sdlWindow, &w, &h);
        glViewport(0, 0, w, h);
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


Mesh Window::UploadMesh(float vertices[], size_t vertexCount) {
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Upload the correct number of bytes
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return Mesh({VAO, VBO});
}



GLuint Window::UploadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cerr << "Failed to load texture: " << IMG_GetError() << "\n";
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum format = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0,
                 format, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);
    return texture;
}
