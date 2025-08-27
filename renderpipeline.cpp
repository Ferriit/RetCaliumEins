#include "headers/renderpipeline.hpp"
#include "headers/window.hpp"


pipeline::pipeline(Window* GameWindow, float fov,
        float nearPlane, float farPlane)
: GameWindow(GameWindow), fov(fov), nearPlane(nearPlane), farPlane(farPlane) {};


void pipeline::init(const char* VertexShaderPath, const char* FragmentShaderPath, Window& Context) {
    this->GameWindow = &Context;

    if (this->GameWindow->init() != 0) exit(-1);

    if (this->GameWindow->GraphicsAPI == API::OPENGL) {
        // Compile shaders
        this->GameWindow->AddShader(VertexShaderPath, RCEnum::RC_VERT);
        this->GameWindow->AddShader(FragmentShaderPath, RCEnum::RC_FRAG);
        this->GameWindow->AddShader("", RCEnum::RC_COMPILE);

        // Store the linked program
        this->ShaderProgram.GL_SHAD = this->GameWindow->Shaders.GL_SHAD;

        // Use the shader
        this->GameWindow->UseShader(this->ShaderProgram);

        // Bind textures to slots (after shader is active)
        glUniform1i(glGetUniformLocation(this->ShaderProgram.GL_SHAD, "uAlbedo"), 0);
        glUniform1i(glGetUniformLocation(this->ShaderProgram.GL_SHAD, "uNormal"), 1);
        glUniform1i(glGetUniformLocation(this->ShaderProgram.GL_SHAD, "uSpecular"), 2);
        glUniform1i(glGetUniformLocation(this->ShaderProgram.GL_SHAD, "uMetallic"), 3);
        glUniform1i(glGetUniformLocation(this->ShaderProgram.GL_SHAD, "uEmission"), 4);
        
        this->aspect = this->GameWindow->Width / (float)this->GameWindow->Height;
    }
}



void pipeline::render() {
    this->GameWindow->clear(); // Clears color and depth buffer

    // Use the shader
    if (this->GameWindow->GraphicsAPI == API::OPENGL) {
        GLuint shader = this->ShaderProgram.GL_SHAD;
        glUseProgram(shader);

        // Camera/view/projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(this->fov), this->aspect, this->nearPlane, this->farPlane);
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f); // Example camera
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, up);

        // Uniform locations
        GLuint uProjectionLoc = glGetUniformLocation(shader, "uProjection");
        GLuint uViewLoc       = glGetUniformLocation(shader, "uView");
        GLuint uModelLoc      = glGetUniformLocation(shader, "uModel");

        glUniformMatrix4fv(uProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Render each object
        for (const auto& obj : this->WorldObjects) {
            // Model matrix
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(obj.Transform.x, obj.Transform.y, obj.Transform.z));
            model = glm::rotate(model, glm::radians(obj.Rotation.x), glm::vec3(1,0,0));
            model = glm::rotate(model, glm::radians(obj.Rotation.y), glm::vec3(0,1,0));
            model = glm::rotate(model, glm::radians(obj.Rotation.z), glm::vec3(0,0,1));
            model = glm::scale(model, glm::vec3(obj.Scale.x, obj.Scale.y, obj.Scale.z));

            glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // Bind VAO
            glBindVertexArray(obj.Mesh.mesh.GL_VAO);

            // Bind textures to appropriate slots
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, obj.Mesh.textures.Albedo.GL_TEX);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, obj.Mesh.textures.Normal.GL_TEX);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, obj.Mesh.textures.Specular.GL_TEX);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, obj.Mesh.textures.Metallic.GL_TEX);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, obj.Mesh.textures.Emission.GL_TEX);

            // Draw
            glDrawArrays(GL_TRIANGLES, 0, obj.Mesh.vertexCount); // Make sure vertexCount exists

            glBindVertexArray(0);
        }

        glUseProgram(0);
    }

    this->GameWindow->update(); // Swap buffers to display frame
}