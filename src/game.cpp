#include "header/game.h"
#include "header/block.h"
#include "header/texture_atlas.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "header/vertex_array.h"
#include <algorithm>

Game* Game::instance = NULL;

Game::Game(int width, int height) {
    Game::instance = this;
    this->width = width;
    this->height = height;
    this->deltaTime = 0;
    this->wireframe = false;
    this->camera = new Camera(glm::vec3(0.0f, Chunk::C_HEIGHT, 0), glm::vec3(0.0f, 1.0f, 0.0f));
    this->window = new Window(width, height, "Voxel game", __framebuffer_size_callback);
    this->projection = glm::perspective(glm::radians(75.0f), this->width / (float)this->height, 0.1f, 100.0f);
    const int SHADOW_WIDTH = 1024*6, SHADOW_HEIGHT = 1024*6;
    this->depthBuffer = new Framebuffer(SHADOW_WIDTH, SHADOW_HEIGHT, true, false);
    this->bloomBuffer = new Framebuffer(512, 512, true, true);
    this->reflectionBuffer = new Framebuffer(512, 512, true, true);
    this->volumeBuffer = new Framebuffer(512, 512, true, true);

    this->skybox = new Skybox({
        "res/images/right.jpg",
        "res/images/left.jpg",
        "res/images/top.jpg",
        "res/images/bottom.jpg",
        "res/images/front.jpg",
        "res/images/back.jpg",
    });

    glfwSetCursorPosCallback(window->getWindow(), __mouse_callback);

    TextureAtlas* atlas = new TextureAtlas();

    this->blockShader = new Shader("res/shader/block.vs", "res/shader/block.fs");
    this->blockShader->use();
    this->blockShader->setInt("texture1", 0);
    this->blockShader->setInt("skybox", 1);
    this->blockShader->setInt("depthMap", 2);
    this->blockShader->setInt("volumeMap", 5);

    this->leafShader = new Shader("res/shader/leaf.vs", "res/shader/block.fs");
    this->leafShader->use();
    this->leafShader->setInt("texture1", 0);
    this->leafShader->setInt("skybox", 1);
    this->leafShader->setInt("depthMap", 2);

    this->reflectionShader = new Shader("res/shader/reflection.vs", "res/shader/reflection.fs");
    this->reflectionShader->use();
    this->reflectionShader->setInt("texture1", 0);
    this->reflectionShader->setInt("skybox", 1);
    this->reflectionShader->setInt("depthMap", 2);

    this->waterShader = new Shader("res/shader/water.vs", "res/shader/water.fs");
    this->waterShader->use();
    this->waterShader->setInt("texture1", 0);
    this->waterShader->setInt("skybox", 1);
    this->waterShader->setInt("depthMap", 2);
    this->waterShader->setInt("reflectionMap", 3);
    this->waterShader->setInt("dudvMap", 4);

    this->depthShader = new Shader("res/shader/depth.vs", "res/shader/depth.fs");

    this->volumeShader = new Shader("res/shader/volume.vs", "res/shader/volume.fs");
    this->volumeShader->use();
    this->volumeShader->setInt("texture1", 0);
    this->volumeShader->setInt("depthMap", 2);


    //TODO: generate terrain
    for (int x = 0; x < RENDER_DISTANCE*Chunk::C_WIDTH; x+=Chunk::C_WIDTH) {
        for (int z = 0; z < RENDER_DISTANCE*Chunk::C_WIDTH; z+=Chunk::C_WIDTH) {
            Chunk* new_chunk = new Chunk(glm::vec3(x, 0, z), this);
            this->chunks.push_back(new_chunk);
        }
    }

    dudvMap = Texture2D("res/images/dudvmap.png", GL_LINEAR);
}

void Game::updateChunks() {
    Chunk* neighbour;

    const glm::vec3 directions[] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
    };

    for (unsigned int i = 0; i < this->chunks.size(); i++) {
        glm::vec3 distance = this->camera->getPosition() - this->chunks[i]->getPos();
        for (const glm::vec3 dir : directions) {
            glm::vec3 out = glm::vec3(RENDER_DISTANCE * Chunk::C_WIDTH * 0.5f);
            glm::vec3 comp = glm::greaterThan(distance * dir, out);
            if (comp.x || comp.z) {
                glm::vec3 pos = this->chunks[i]->getPos();
                glm::vec3 out = dir;
                out *= RENDER_DISTANCE * Chunk::C_WIDTH;
                pos += out;
                this->chunks[i]->load(pos);
                neighbour = this->getChunkAt(pos.x - dir.x * Chunk::C_WIDTH, pos.y, pos.z - dir.z * Chunk::C_WIDTH);
                if (neighbour)
                    neighbour->setShouldUpdate(true);
            }
        }
    }
}

void Game::start() {
    double lastTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE); 
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    glEnable(GL_FRAMEBUFFER_SRGB);

    VertexArray<float> quad({
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    });
    quad.load();
    quad.setAttribute(0, 3, GL_FLOAT, 5, 0);
    quad.setAttribute(1, 2, GL_FLOAT, 5, 3);
    quad.unbind();
        
    unsigned int fps = 0;
    double fpsTime = glfwGetTime();

    Shader volumeQuad("res/shader/quad.vs", "res/shader/volumeQuad.fs");
    Shader bloomQuad("res/shader/quad.vs", "res/shader/bloom.fs");

    this->setupShader(this->blockShader);
    this->setupShader(this->waterShader);
    this->waterShader->setVec2("screenSize", glm::vec2(this->width, this->height));
    this->setupShader(this->leafShader);
    this->setupShader(this->reflectionShader);
    this->setupShader(this->volumeShader);

    while(window->isRunning()) {

        fps++;

        if (glfwGetTime() - fpsTime > 1) {
            std::cout << "FPS: " << fps << std::endl;
            fps = 0;
            fpsTime = glfwGetTime();
        }

        this->deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
        
        this->processInput();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        this->render();

        glViewport(0, 0, this->width, this->height);    

        //TODO: render post processing
        glDisable(GL_DEPTH_TEST);
        bloomQuad.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->bloomBuffer->getColorMap());
        quad.render(GL_TRIANGLE_STRIP);
        volumeQuad.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->volumeBuffer->getColorMap());
        quad.render(GL_TRIANGLE_STRIP);
        glEnable(GL_DEPTH_TEST);
        window->update();
    }
}

void Game::render() {
    this->updateChunks();

    std::vector<std::pair<int, float>> indices;  

    glm::vec3 pos = this->getChunkAt(this->camera->getPosition())->getPos();

    for (unsigned int i = 0; i < this->chunks.size(); i++) {
        glm::vec3 chunkPos = this->chunks[i]->getPos();
        indices.push_back({i, glm::distance(pos, chunkPos)});
    }        

    std::sort(indices.begin(), indices.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    //TODO: render scene for depth buffer
    float near_plane = 0.001f, far_plane = 200.0f;
    glm::vec3 lightPos = glm::vec3(-13.0f, 25.0f, -13.0f);

    float angle = glfwGetTime() * 0.01;
    lightPos = glm::rotateY(lightPos, angle);
    glm::mat4 lightSpaceMatrix;
    lightSpaceMatrix = this->getLightSpaceMatrix(lightPos, near_plane, far_plane, 75.0f);
    lightPos = -lightPos;

    glActiveTexture(GL_TEXTURE0);
    TextureAtlas::instance->bind();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->skybox->texture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->depthBuffer->getDepthMap());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, this->reflectionBuffer->getColorMap());

    glActiveTexture(GL_TEXTURE4);
    this->dudvMap.bind();

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, this->volumeBuffer->getDepthMap());

    this->renderDepthmap(lightSpaceMatrix);

    this->updateShader(blockShader, lightSpaceMatrix, lightPos);
    this->updateShader(reflectionShader, lightSpaceMatrix, lightPos);
    this->updateShader(waterShader, lightSpaceMatrix, lightPos);
    this->waterShader->setFloat("time", glfwGetTime());
    this->updateShader(leafShader, lightSpaceMatrix, lightPos);
    this->leafShader->setFloat("time", glfwGetTime());
    this->updateShader(volumeShader, lightSpaceMatrix, lightPos);

    //TODO: render scene for volumetric light
    this->volumeBuffer->use();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(const auto& chunk : this->chunks) {
        chunk->render(this->volumeShader);
        chunk->renderTransparent(this->volumeShader);
        chunk->renderWater(this->volumeShader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->reflectionBuffer->use();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    for(const auto& pair : indices) {
        Chunk* chunk = this->chunks[pair.first];
        chunk->render(this->reflectionShader);
        chunk->renderTransparent(this->reflectionShader);
    }
    glCullFace(GL_FRONT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //TODO: render bloom
    this->bloomBuffer->use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->skybox->render(this->camera, this->projection);
    this->waterShader->use();
    this->waterShader->setVec2("screenSize", glm::vec2(512, 512));

    for(const auto& pair : indices) {
        Chunk* chunk = this->chunks[pair.first];
        chunk->render(blockShader);
        chunk->renderWater(waterShader);
        chunk->renderTransparent(this->leafShader);
    }
        
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // TODO: render as usual
    glViewport(0, 0, this->width, this->height);
    this->waterShader->use();
    this->waterShader->setVec2("screenSize", glm::vec2(width, height));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->skybox->render(this->camera, this->projection);

    bool flag = false;
    for(const auto& pair : indices) {
        Chunk* chunk = this->chunks[pair.first];
        if (chunk->getShouldUpdate() && !flag) {
            chunk->setShouldUpdate(false);
            chunk->generateMesh();
            chunk->generateBuffer();
            flag = true;
        }
        chunk->render(blockShader);
        chunk->renderWater(waterShader);
        chunk->renderTransparent(leafShader);
    }
    
}

Game::~Game() {
    delete this->skybox;
    delete this->blockShader;
    delete this->leafShader;
    delete this->depthShader;
    delete this->bloomBuffer;
    delete this->depthBuffer;
    delete this->reflectionBuffer;
    delete this->reflectionShader;
    for (Chunk* chunk : this->chunks)
        delete chunk;
    delete this->camera;
    delete this->window;
}

void Game::__framebuffer_size_callback(GLFWwindow*, int width, int height) {
    Game::instance->framebuffer_size_callback(width, height);
}

void Game::__mouse_callback(GLFWwindow*, double x, double y) {
    Game::instance->mouse_callback(x, y);
}

void Game::framebuffer_size_callback(int width, int height) {
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
    this->projection = glm::perspective(glm::radians(75.0f), this->width / (float)this->height, 0.1f, 100.0f);
    this->setupShader(this->blockShader);
    this->setupShader(this->waterShader);
    this->waterShader->setVec2("screenSize", glm::vec2(this->width, this->height));
    this->setupShader(this->reflectionShader);
    this->setupShader(this->leafShader);
    this->setupShader(this->volumeShader);
}

void Game::mouse_callback(double x, double y) {
    this->camera->processMouse(x, y);
}

void Game::processInput() {
    GLFWwindow* window = this->window->getWindow();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) 
        glfwSetWindowShouldClose(window, true);    

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        this->camera->processKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        this->camera->processKeyboard(BACK, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        this->camera->processKeyboard(UP, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        this->camera->processKeyboard(DOWN, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        this->camera->processKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        this->camera->processKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        this->wireframe = !this->wireframe;
        if (this->wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

Block& Game::getBlockAt(glm::vec3 pos) {
    for (Chunk* c : this->chunks) {
        if (glm::all(glm::greaterThanEqual(pos, c->getPos())) && glm::all(glm::lessThan(pos, c->getPos() + Chunk::CHUNK_SIZE))) {
            return c->getBlockAt(pos - c->getPos());
        }
    }
    return Block::invalid;
}

Block& Game::getBlockAt(const int x, const int y, const int z) {
    return this->getBlockAt(glm::vec3(x, y, z));
}

Chunk* Game::getChunkAt(const int x, const int y, const int z) {
    glm::vec3 pos(x, y, z);
    for (Chunk* c : this->chunks) 
        if (c->getPos() == pos) 
            return c;
    return this->chunks[0];
}

Chunk* Game::getChunkAt(glm::vec3 pos) {
    for (Chunk* c : this->chunks) 
        if (glm::all(glm::greaterThanEqual(pos, c->getPos())) && glm::all(glm::lessThan(pos, c->getPos() + Chunk::CHUNK_SIZE))) 
            return c;
    return this->chunks[0];
}

void Game::setupShader(Shader* shader) {
    shader->use();
    shader->setMat4("projection", this->projection);
    glm::vec3 lightColor(1.0, 0.89, 0.89);
    shader->setVec3("light.ambient", lightColor * 0.3f);
    shader->setVec3("light.diffuse", lightColor * 0.5f);
    shader->setVec3("light.specular", lightColor * 1.0f);
}

void Game::updateShader(Shader* shader, glm::mat4& lightSpace, glm::vec3& lightPos) {
    shader->use();
    shader->setVec3("viewPos", camera->getPosition());
    shader->setMat4("view", camera->getViewMatrix());
    shader->setMat4("lightSpaceMatrix", lightSpace);
    shader->setVec3("light.direction", lightPos);
}

void Game::renderDepthmap(glm::mat4& lightSpace) {
    this->depthShader->use();
    this->depthShader->setMat4("lightSpaceMatrix", lightSpace);
    this->depthBuffer->use();
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->depthBuffer->getDepthMap());
    for (Chunk* c : this->chunks) {
        c->render(depthShader);
        c->renderTransparent(depthShader);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 Game::getLightSpaceMatrix(glm::vec3& pos, float near, float far, float size) {
    glm::mat4 lightProjection = glm::ortho(-size, size, -size, size, near, far);  

    return lightProjection * glm::lookAt(
        pos + camera->getPosition(),
        camera->getPosition(),
        glm::vec3(0.0, 1.0, 0.0)
    );
}