#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "camera.h"
#include "shader.h"
#include "stb_image.h"
#include "vertex_array.h"
#include <vector>
#include <string>

class Camera;

class Skybox {
    private:
        VertexArray<float> mesh;
        Shader* shader;
    public:
        unsigned int texture;
        Skybox(std::vector<std::string> images);
        ~Skybox();
        void render(const Camera* camera, const glm::mat4& projection);
};

#endif