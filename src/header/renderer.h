#ifndef RENDERER_H
#define RENDERER_H

#include "skybox.h"
#include "chunk.h"
#include "shader.h"
#include "camera.h"
#include <algorithm>
#include <string>
#include <memory>

class Chunk;

class Renderer {
    protected:
        std::shared_ptr<Shader> solid, water, transparent;
        void updateShader(std::shared_ptr<Shader> & shader, Camera* camera, glm::mat4& lightSpaceMatrix, glm::vec3& lightPos);
        void setupShader(std::shared_ptr<Shader> & shader, glm::mat4& projection);
        void setDefaultUniforms(std::shared_ptr<Shader> & shader);

    public:
        Renderer() {};
        virtual ~Renderer() {};
        Renderer(std::shared_ptr<Shader> solid, std::shared_ptr<Shader> water, std::shared_ptr<Shader> transparent);

        virtual void render(const std::vector<std::pair<Chunk*, float>>& chunks, Skybox* skybox=NULL);
        virtual void updateShaders(Camera* camera, glm::mat4& lightSpaceMatrix, glm::vec3& lightPos);
        virtual void updateShaders(Camera* camera, glm::mat4& lightSpaceMatrix, glm::vec3& lightPos, glm::vec2 screenSize);
        virtual void setupShaders(glm::mat4& projection);

        Shader& getShaderSolid();
        Shader& getShaderWater();
        Shader& getShaderTransparent();
};

#endif