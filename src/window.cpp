#include "header/window.h"
#include <iostream>


Window::Window(int width, int height, const char* title, callback_func callback) {
    this->title = title;    

    glfwInit();

    // tell the version we are running
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

    //only use core features of OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (window == NULL) {
        std::cout << "Was not able to create a GLFW context" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init glad" << std::endl;
    }

    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
}

Window::~Window() {
    glfwTerminate();
}

GLFWwindow* Window::getWindow() const  {
    return this->window;
}

bool Window::isRunning() {
    return !glfwWindowShouldClose(this->window);
}

void Window::update() {
    // glfwSwapBuffers(this->window);
    glFlush();
    glfwPollEvents();
}