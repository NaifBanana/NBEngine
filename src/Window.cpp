#include "Window.h"
namespace NB {

std::atomic<uint8_t> Window::windowCount(0);

Window::Window(const uint16_t x, const uint16_t y, const char* initName, GLFWmonitor* initMonitor, GLFWwindow* initWindow) {
    if (windowCount == 0) {
        glfwInit();
    }

    windowSize = {x, y};
    windowName = std::string(initName);
    monitor = initMonitor;
    shareWindow = initWindow;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowSize[0], windowSize[0], windowName.c_str(), monitor, shareWindow);
    if (window == NULL) {
        std::cout << "NB::NBWINDOW::COULD NOT CREATE WINDOW(" << windowName << ")\n";
        if (windowCount == 0) { glfwTerminate(); }
        ready = false;
    } else {
        windowCount++;
        ready = true;
    }

}

Window::Window(const std::array<uint16_t, 2> initSize, const char* initName, GLFWmonitor* initMonitor, GLFWwindow* initWindow) :
    Window(initSize[0], initSize[1], initName, initMonitor, initWindow){}

Window::~Window() {
    glfwSetWindowShouldClose(window, true);
    glfwDestroyWindow(window);
    windowCount--;
    if (windowCount == 0) {
        glfwTerminate();
    }
}

int Window::init() {
    if (!ready) {
        std::cout << "NB::NBWINDOW::NOT READY\n";
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "NB::NBWINDOW::COULD NOT INITIALIZE GLAD\n";
        if (windowCount == 0) { glfwTerminate(); }
        return -1;
    }

    glViewport(0, 0, windowSize[0], windowSize[1]);
    running=true;
    return 0;
}

GLFWwindow* Window::getWindow() const {
    return window;
}

std::array<uint16_t, 2> Window::getSize() const {
    return windowSize;
}

std::string Window::getName() const {
    return windowName;
}

void Window::resize(const std::array<uint16_t, 2> newSize) {
    windowSize = newSize;
    if (running) {
        glViewport(0, 0, windowSize[0], windowSize[1]);
        glfwSetWindowSize(window, windowSize[0], windowSize[1]);
    }
}

void Window::resize(const uint16_t x, const uint16_t y) {
    windowSize = {x, y};
    if (running) {
        glViewport(0, 0, windowSize[0], windowSize[1]);
        glfwSetWindowSize(window, windowSize[0], windowSize[1]);
    }
}

};