#pragma once
#ifndef _NB_WINDOW
#define _NB_WINDOW

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <array>
#include <string>
#include <iostream>
namespace NB {

class Window {
public:
    Window(const std::array<uint16_t, 2>, const char*, GLFWmonitor* initMonitor=NULL, GLFWwindow* initWindow=NULL);
    Window(const uint16_t, const uint16_t, const char*, GLFWmonitor* initMonitor=NULL, GLFWwindow* initWindow=NULL);
    ~Window();


    int init();
    GLFWwindow* getWindow() const;
    std::array<uint16_t, 2> getSize() const;
    std::string getName() const;
    void resize(const std::array<uint16_t, 2>);
    void resize(const uint16_t x, const uint16_t y);

protected:
    static std::atomic<uint8_t> windowCount;

    std::array<uint16_t, 2> windowSize;
    std::string windowName;
    bool ready=false, running=false;
    GLFWwindow* window;
    GLFWmonitor* monitor;
    GLFWwindow* shareWindow;
};

};
#endif