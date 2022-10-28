#ifndef GAME_H
#define GAME_H

#include "gfx/renderer/renderer_fbo.h"
#include "gfx/framebuffer.h"
#include "gfx/texture2d.h"
#include "gfx/skybox.h"
#include "gfx/window.h"
#include "world/chunk.h"
#include "util/camera.h"
#include "entity/entity.h"

class Game {
    private:
        constexpr static int RENDER_DISTANCE = 16;
        constexpr static float NEAR_PLANE = 0.1, FAR_PLANE = 150, FOV = 75.0, SHADOW_SIZE = 50.0;
        constexpr static int SHADOW_WIDTH = 1024*4, SHADOW_HEIGHT = 1024*4;
        Chunk *prevChunk;
        Camera *camera;
        Window window;
        Renderer *worldRenderer;
        RendererFBO *volumetricRenderer, *depthRenderer, *reflectionRenderer, *bloomRenderer;
        Texture2D dudvMap;
        float deltaTime;
        int width, height;
        glm::mat4 projection;
        bool wireframe;
        std::vector<Chunk*> chunks;
        Entity player;
        void updateChunks();
        glm::mat4 getLightSpaceMatrix(glm::vec3& pos, float near, float far, float size);
    
    public:
        static Game* instance;
        Skybox* skybox;
        
        Game(int width, int height);
        ~Game();
        void framebuffer_size_callback(int width, int height);
        void processInput();
        void mouse_callback(double x, double y);
        void start();
        void render();
        Block& getBlockAt(const int x, const int y, const int z);
        Block& getBlockAt(glm::vec3 pos);
        Chunk* getChunkAt(const int x, const int y, const int z);
        Chunk* getChunkAt(glm::vec3 pos);
        Hit rayCast(float distance);
        Hit rayCast(Ray ray);
        const std::vector<Chunk*>& getChunks();
        static void __framebuffer_size_callback(GLFWwindow* window, int width, int height);  
        static void __mouse_callback(GLFWwindow* window, double x, double y);
};

#endif
