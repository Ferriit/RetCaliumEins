#include "headers/window.hpp"


// OpenGL Specifics
GLuint CompileShaderGL(GLenum Type, const char* Source) {
    GLuint Shader = glCreateShader(Type);
    glShaderSource(Shader, 1, &Source, nullptr);
    glCompileShader(Shader);

    GLint Success;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
    if (!Success) {
        char InfoLog[512];
        glGetShaderInfoLog(Shader, 512, nullptr, InfoLog);
        std::cerr << "Shader compilation failed:\n" << InfoLog << "\n";
    }
    return Shader;
}


GLuint CreateShaderProgramGL(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader = CompileShaderGL(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = CompileShaderGL(GL_FRAGMENT_SHADER, fragmentSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed:\n" << infoLog << "\n";
    }

    // Clean up shaders (no longer needed once linked)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

std::string LoadSource(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << "\n";
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


// Windowing initialization and functions
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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


RCMesh Window::UploadMesh(RawMeshRC mesh) {
    RCMesh output;
    output.vertexCount = mesh.vertexCount;

    if (this->GraphicsAPI == API::OPENGL) {
        // --- Upload Vertex Data ---
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertexCount * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        output.mesh.GL_VAO = VAO;
        output.mesh.GL_VBO = VBO;

        // --- Link already uploaded textures ---
        output.textures.Albedo = UploadTexture(mesh.textures.AlbedoPath);
        output.textures.Emission = UploadTexture(mesh.textures.EmissionPath);
        output.textures.Metallic = UploadTexture(mesh.textures.MetallicPath);
        output.textures.Normal = UploadTexture(mesh.textures.NormalPath);
        output.textures.Specular = UploadTexture(mesh.textures.SpecularPath);

        return output;
    }


    output.mesh.ErrorCode = 1;
    output.UseDefaultProgram = true;
    return output;
}


RCuint Window::UploadTexture(const char* path) {
    if (this->GraphicsAPI == API::OPENGL) {
        SDL_Surface* surface = IMG_Load(path);
        if (!surface) {
            std::cerr << "Failed to load texture: " << IMG_GetError() << "\n";
            return RCuint({.ErrorCode=1});
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
        RCuint TEX;
        TEX.GL_TEX = texture;
        TEX.ErrorCode = 0;
        return TEX;
    }

    return RCuint({.ErrorCode=1});
}


RCMesh Window::LoadOBJ(const std::string& filename, TextureSuiteRC tex) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << "\n";
        exit(1);
    }

    std::vector<Vertex> vertices;
    std::vector<float> x_list, y_list, z_list;
    std::vector<int> face_indices;

    std::string line;
    // First pass: read vertices
    while (std::getline(file, line)) {
        if (line.substr(0, 2) == "v ") {
            float x, y, z;
            sscanf(line.c_str() + 2, "%f %f %f", &x, &y, &z);
            vertices.push_back({x, y, z, 0.f, 0.f, 0.f});
            x_list.push_back(x);
            y_list.push_back(y);
            z_list.push_back(z);
        }
    }

    if (vertices.empty()) {
        std::cerr << "OBJ file has no vertices!\n";
        exit(1);
    }

    // Compute bounding box for centering & color
    float min_x = *std::min_element(x_list.begin(), x_list.end());
    float max_x = *std::max_element(x_list.begin(), x_list.end());
    float min_y = *std::min_element(y_list.begin(), y_list.end());
    float max_y = *std::max_element(y_list.begin(), y_list.end());
    float min_z = *std::min_element(z_list.begin(), z_list.end());
    float max_z = *std::max_element(z_list.begin(), z_list.end());

    float cx = (min_x + max_x) / 2.f;
    float cy = (min_y + max_y) / 2.f;
    float cz = (min_z + max_z) / 2.f;

    float dx = (max_x - min_x == 0.f) ? 1.f : (max_x - min_x);
    float dy = (max_y - min_y == 0.f) ? 1.f : (max_y - min_y);
    float dz = (max_z - min_z == 0.f) ? 1.f : (max_z - min_z);

    // Center vertices & assign RGB
    for (auto& v : vertices) {
        v.x -= cx;
        v.y -= cy;
        v.z -= cz;

        v.r = (v.x - min_x) / dx;
        v.g = (v.y - min_y) / dy;
        v.b = (v.z - min_z) / dz;
    }

    // Second pass: read faces
    file.clear();
    file.seekg(0);
    while (std::getline(file, line)) {
        if (line.substr(0, 2) == "f ") {
            int i1, i2, i3;
            sscanf(line.c_str() + 2, "%d%*s %d%*s %d%*s", &i1, &i2, &i3);
            // OBJ indices are 1-based
            face_indices.push_back(i1 - 1);
            face_indices.push_back(i2 - 1);
            face_indices.push_back(i3 - 1);
        }
    }
    file.close();

    // Flatten vertices for GPU upload
    std::vector<float> vertex_data;
    for (size_t i = 0; i < face_indices.size(); i++) {
        Vertex& v = vertices[face_indices[i]];
        vertex_data.push_back(v.x);
        vertex_data.push_back(v.y);
        vertex_data.push_back(v.z);
        vertex_data.push_back(v.r);
        vertex_data.push_back(v.g);
        vertex_data.push_back(v.b);
    }

    // Upload to GPU and return the RCuint handle

    std::vector<float> DataVec(vertex_data.data(), vertex_data.data() + vertex_data.size());
    RawMeshRC out = { 
        .textures=tex, 
        .vertexCount=vertex_data.size() * sizeof(float), 
        .vertices=DataVec};
    return this->UploadMesh(out);
}

RCuint Window::AddShader(const char* filename, RCEnum Type) {
    std::string Shader = LoadSource(filename);

    if (this->GraphicsAPI == API::OPENGL) {
        switch (Type) {
            case RCEnum::RC_VERT:
                this->RawShaders.Vert = Shader;
                break;
            
            case RCEnum::RC_FRAG:
                this->RawShaders.Frag = Shader;
                break;
            
            case RCEnum::RC_COMPILE:
                this->Shaders.GL_SHAD = CreateShaderProgramGL(RawShaders.Vert.c_str(), RawShaders.Frag.c_str());

            default:
                std::cout << "Invalid RCEnum type. Must be either RC_VERT or RC_FRAG, RC_COMPILE to compile" << std::endl;
                return RCuint({.ErrorCode=1});
        }
    }
    return RCuint({.ErrorCode=0});
}

void Window::UseShader(RCuint ShaderProgram) {
    if (this->GraphicsAPI == API::OPENGL) {
        glUseProgram(ShaderProgram.GL_SHAD);
    }
}
