#ifndef WINDOW_H
#define WINDOW_H

#include "glad.h"
#include <GLFW/glfw3.h>

typedef void (*callback_func)(GLFWwindow*, int, int);

class Window {
    private:
        const char* title;
        GLFWwindow* window;

    public:
        Window(int width, int height, const char* title, callback_func callback);
        ~Window();
        bool isRunning();
        void update();
        GLFWwindow* getWindow() const;
};

#endif