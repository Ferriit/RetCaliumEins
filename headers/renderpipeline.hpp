#pragma once
#include "window.hpp"


struct Vector3 {
    float x, y, z;
};

struct WorldObject {
    RCMesh Mesh;
    Vector3 Transform;
    Vector3 Rotation;
    Vector3 Scale;
};


class pipeline {
    public:
        std::vector<WorldObject*> WorldObjects;
        RCuint ShaderProgram;
        std::vector<RCMaterial> Materials;
        Window* GameWindow;

        float fov;
        float aspect;
        float nearPlane;
        float farPlane;


        pipeline(Window* GameWindow, float fov, float nearPlane, float farPlane);

        void init(const char* VertexShaderPath, const char* FragmentShaderPath, Window& Context);

        void render();
};
