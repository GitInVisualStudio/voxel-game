#ifndef GAME_H
#define GAME_H

#include "chunk.h"
#include "camera.h"
#include "window.h"
#include "skybox.h"
#include "framebuffer.h"
#include "texture2d.h"

class Chunk;
class Shader;

class Game {
    private:
        constexpr static int RENDER_DISTANCE = 14;
        Chunk* prevChunk;
        Camera* camera;
        Window* window;
        Framebuffer* depthBuffer;
        Framebuffer* bloomBuffer;
        Framebuffer* reflectionBuffer;
        Framebuffer* volumeBuffer;
        Shader* blockShader;
        Shader* depthShader;
        Shader* leafShader;
        Shader* waterShader;
        Shader* reflectionShader;
        Shader* volumeShader;
        Texture2D dudvMap;
        float deltaTime;
        int width, height;
        glm::mat4 projection;
        bool wireframe;
        std::vector<Chunk*> chunks;
        void updateChunks();
        void setupShader(Shader* shader);
        void updateShader(Shader* shader, glm::mat4& lightSpace, glm::vec3& lightPos);
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
        static void __framebuffer_size_callback(GLFWwindow* window, int width, int height);  
        static void __mouse_callback(GLFWwindow* window, double x, double y);
};

#endif
