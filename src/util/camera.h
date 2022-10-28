#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

enum Camera_Movement{
    LEFT, 
    RIGHT,
    FORWARD, 
    BACK, 
    UP, 
    DOWN 
};

class Camera{
    private:
        bool init;
        float lastX, lastY, pitch, yaw;
        float prevPitch, prevYaw;
        glm::vec3 position, prevPosition;
        glm::vec3 worldUp;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 dir;

        void updateVectors();
    
    public:
        Camera(glm::vec3 position, glm::vec3 worldUp);
        void processKeyboard(Camera_Movement movement, float deltaTime);
        void processMouse(float x, float y);
        void update(float dt);
        glm::mat4 getViewMatrix() const;
        glm::vec3& getPosition();
        void setPosition(glm::vec3 pos);
        const glm::vec3& getDirection() const;
        const glm::vec3& getRight() const;
        const glm::vec3& getUp() const;
};

#endif