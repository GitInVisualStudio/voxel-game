#include "header/camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 worldUp){
    this->position = position;
    this->worldUp = worldUp;
    this->dir = glm::vec3(0.0f, 0.0f, -1.0f);
    this->yaw = 0;
    this->pitch = 0;
    this->lastX = this->lastY = 0;
    this->init = false;
    updateVectors();
}

void Camera::processKeyboard(Camera_Movement movement, float deltaTime){
    float cameraSpeed = 10.0f * deltaTime;
    
    switch(movement){
        case LEFT:
            this->position -= cameraSpeed * right;
            break;
        case RIGHT:
            this->position += cameraSpeed * right;
            break;
        case FORWARD:
            this->position += cameraSpeed * dir;
            break;
        case BACK:
            this->position -= cameraSpeed * dir;
            break;
        case UP:
            this->position += cameraSpeed * up;
            break;
        case DOWN:
            this->position -= cameraSpeed * up;
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

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch =  89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    
    updateVectors();
}

void Camera::updateVectors(){
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir = glm::normalize(dir);

    right = glm::normalize(glm::cross(this->dir, this->worldUp));
    up = glm::normalize(glm::cross(this->right, dir));
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + dir, up);
}

glm::vec3 Camera::getPosition() const {
    return this->position;
}

glm::vec3 Camera::getDirection() const {
    return this->dir;
}