#ifndef RENDERER_FBO_H
#define RENDERER_FBO_H

#include "renderer.h"
#include "framebuffer.h"

class RendererFBO : public Renderer, public Framebuffer {
    public:
        RendererFBO() {};
        RendererFBO(int width, int height, bool color, bool depth, std::shared_ptr<Shader> solid, std::shared_ptr<Shader> water, std::shared_ptr<Shader> transparent);
        virtual ~RendererFBO() {};

        virtual void render(const std::vector<std::pair<Chunk*, float>>& chunks, Skybox* skybox=NULL);
        virtual void updateShaders(Camera* camera, glm::mat4& lightSpaceMatrix, glm::vec3& lightPos);

};

#endif