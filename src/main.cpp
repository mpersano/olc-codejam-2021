#include "vulkandevice.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

class Demo
{
public:
    Demo();
    ~Demo();

    void initialize(int width, int height, const char *title);
    void terminate();

    void renderLoop();

private:
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void keyEvent(int key, int scancode, int action, int mods);

    GLFWwindow *m_window = nullptr;
    std::unique_ptr<VulkanDevice> m_vulkanDevice;
};

Demo::Demo()
{
    glfwInit();
    glfwSetErrorCallback([](int error, const char *description) {
        std::cerr << "GLFW error " << error << ": " << description << '\n';
    });
}

Demo::~Demo()
{
    terminate();
}

void Demo::initialize(int width, int height, const char *title)
{
    m_vulkanDevice.reset(new VulkanDevice);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, Demo::keyCallback);
}

void Demo::terminate()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Demo::renderLoop()
{
    while (!glfwWindowShouldClose(m_window)) {
        // XXX rendering
        glfwPollEvents();
    }
}

void Demo::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto *demo = reinterpret_cast<Demo *>(glfwGetWindowUserPointer(window));
    demo->keyEvent(key, scancode, action, mods);
}

void Demo::keyEvent(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(m_window, 1);
}

int main()
{
    Demo demo;
    demo.initialize(1200, 600, "game");
    demo.renderLoop();
}
