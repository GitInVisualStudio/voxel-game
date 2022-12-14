#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "shader.h"
#include "vertex_array.h"
#include <vector>
#include <string>

class Camera;

class Skybox {
    private:
        VertexArray<float> mesh;
        Shader shader;
        Camera* camera;
        glm::mat4* projection;
        unsigned int texture;
    public:
        Skybox(std::vector<std::string> images, Camera* camera, glm::mat4* projection);
        ~Skybox();
        void render();
        void bind(int id=-1);
};

#endif