#include "camera.h"

// #define SMOOTH_CAMERA

Camera::Camera(glm::vec3 position, glm::vec3 worldUp){
    this->position = position;
    this->prevPosition = position;
    this->worldUp = worldUp;
    this->dir = glm::vec3(0.0f, 0.0f, -1.0f);
    this->yaw = this->pitch = this->prevPitch = this->prevYaw = 0;
    this->lastX = this->lastY = 0;
    this->init = false;
    updateVectors();
}

void Camera::processKeyboard(Camera_Movement movement, float deltaTime){
    float cameraSpeed = 15.0f * deltaTime;
    
    switch(movement){
        case LEFT:
            this->prevPosition -= cameraSpeed * right;
            break;
        case RIGHT:
            this->prevPosition += cameraSpeed * right;
            break;
        case FORWARD:
            this->prevPosition += cameraSpeed * dir;
            break;
        case BACK:
            this->prevPosition -= cameraSpeed * dir;
            break;
        case UP:
            this->prevPosition += cameraSpeed * up;
            break;
        case DOWN:
            this->prevPosition -= cameraSpeed * up;
            break;
    }
}

void Camera::processMouse(float x, float y){
    float xoffset = x - lastX;
    float yoffset = lastY - y; // reversed since y-coordinates range from bottom to top
    lastX = x;
    lastY = y;

    if(!init){
        init = true;
        return;
    }

    const float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    prevYaw += xoffset;
    prevPitch += yoffset;

    if(prevPitch > 89.0f)
        prevPitch =  89.0f;
    if(prevPitch < -89.0f)
        prevPitch = -89.0f;
    
    updateVectors();
}

void Camera::updateVectors(){
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir = glm::normalize(dir);

    right = glm::normalize(glm::cross(this->dir, this->worldUp));
    // up = glm::normalize(glm::cross(this->right, dir));
    up = this->worldUp;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + dir, up);
}

glm::vec3& Camera::getPosition() {
    return this->position;
}

void Camera::setPosition(glm::vec3 pos) {
    this->prevPosition = pos;
}

const glm::vec3& Camera::getDirection() const {
    return this->dir;
}

const glm::vec3& Camera::getRight() const {
    return this->right;
}

const glm::vec3& Camera::getUp() const {
    return this->up;
}

void Camera::update(float dt) {
    # ifdef SMOOTH_CAMERA
        this->pitch -= (this->pitch - this->prevPitch) * dt;
        this->yaw -= (this->yaw - this->prevYaw) * dt;
        this->position -= (this->position - this->prevPosition) * dt;
        this->updateVectors();
    #else
        this->pitch = this->prevPitch;
        this->yaw = this->prevYaw;
        this->position = this->prevPosition;
    #endif
}