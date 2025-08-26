#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <SDL2/SDL_opengl.h>
#include <system_error>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>


enum class API {
    OPENGL,
    VULKAN
};

struct Vertex {
    float x, y, z;
    float r, g, b;
};

struct RCuint {
    // OpenGL handles
    GLuint GL_VAO        = 0; // Vertex Array Object
    GLuint GL_VBO        = 0; // Vertex Buffer Object
    GLuint GL_EBO        = 0; // Index Buffer Object
    GLuint GL_TEX        = 0; // Texture
    GLuint GL_SHAD       = 0; // Shader Program

    // Vulkan handles
    VkBuffer        VKBUF       = VK_NULL_HANDLE; // Vertex/Index buffer
    VkImage         VKIMG       = VK_NULL_HANDLE; // Image/Texture
    VkImageView     VKIMGVIEW   = VK_NULL_HANDLE; // Image view
    VkDeviceMemory  VKMEM       = VK_NULL_HANDLE; // GPU memory for buffer/image
    VkShaderModule  VKSHADER    = VK_NULL_HANDLE; // Shader module
    VkPipeline      VKPIPELINE  = VK_NULL_HANDLE; // Pipeline
    VkPipelineLayout VKPL       = VK_NULL_HANDLE; // Pipeline layout

    int ErrorCode               = 0;
};

struct RCTexture {
    RCuint Albedo, Normal, Specular, Metallic, Emission;
};

struct RCMesh {
    RCTexture textures;
    RCuint mesh;
    size_t vertexCount;
};

enum class RCEnum {
    // Shader types
    RC_VERT,
    RC_FRAG,
    RC_COMPILE,

    // Draw modes
    TRIANGLES,       // GL_TRIANGLES / VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    LINES,           // GL_LINES / VK_PRIMITIVE_TOPOLOGY_LINE_LIST
    POINTS,          // GL_POINTS / VK_PRIMITIVE_TOPOLOGY_POINT_LIST

    // Texture format
    RGB8,            // GL_RGB8 / VK_FORMAT_R8G8B8_UNORM
    RGBA8,           // GL_RGBA8 / VK_FORMAT_R8G8B8A8_UNORM
    DEPTH24,         // GL_DEPTH_COMPONENT24 / VK_FORMAT_D24_UNORM_S8_UINT
    LINEAR,
    BITWISE
};

struct TextureSuiteRC {
    const char* AlbedoPath;
    const char* NormalPath;
    const char* SpecularPath;
    const char* MetallicPath;
    const char* EmissionPath;
};

struct RawMeshRC {
    TextureSuiteRC textures;
    size_t vertexCount;
    std::vector<float> vertices;
};

struct RawSourceShaders {
    std::string Vert;
    std::string Frag;
};

class Window {
public:
    const char* name;
    int Width;
    int Height;
    API GraphicsAPI;

    SDL_Window* sdlWindow = nullptr;
    SDL_GLContext glContext = nullptr;

    RCuint Shaders;

    RawSourceShaders RawShaders;

    Window(const char* name, int Width, int Height, API GraphicsAPI);
    ~Window();

    int init();

    void clear();

    void SetClearColor(float r, float g, float b, float alpha);

    void update();

    RCMesh UploadMesh(RawMeshRC mesh);
    
    RCuint UploadTexture(const char* path);

    RCMesh LoadOBJ(const std::string& filename, TextureSuiteRC tex);

    RCuint AddShader(const char* filename, RCEnum Type);
};
