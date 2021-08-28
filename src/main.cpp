#include "vulkancommandbuffer.h"
#include "vulkancommandpool.h"
#include "vulkandevice.h"
#include "vulkanpipeline.h"
#include "vulkansemaphore.h"
#include "vulkanshadermodule.h"
#include "vulkansurface.h"
#include "vulkanswapchain.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

class VulkanRenderer : private NonCopyable
{
public:
    explicit VulkanRenderer(GLFWwindow *window, int width, int height);
    ~VulkanRenderer();

    void render() const;

private:
    GLFWwindow *m_window;
    std::unique_ptr<VulkanDevice> m_device;
    std::unique_ptr<VulkanSurface> m_surface;
    std::unique_ptr<VulkanSwapchain> m_swapchain;
    std::unique_ptr<VulkanShaderModule> m_vertexShaderModule;
    std::unique_ptr<VulkanShaderModule> m_fragmentShaderModule;
    std::unique_ptr<VulkanPipeline> m_pipeline;
    std::unique_ptr<VulkanCommandPool> m_commandPool;
    std::unique_ptr<VulkanSemaphore> m_imageAvailableSemaphore;
    std::unique_ptr<VulkanSemaphore> m_renderFinishedSemaphore;
    std::vector<std::unique_ptr<VulkanCommandBuffer>> m_commandBuffers;
};

VulkanRenderer::VulkanRenderer(GLFWwindow *window, int width, int height)
    : m_window(window)
    , m_device(new VulkanDevice)
    , m_surface(new VulkanSurface(m_device.get(), window))
    , m_swapchain(new VulkanSwapchain(m_device.get(), m_surface.get(), width, height, 3))
    , m_vertexShaderModule(new VulkanShaderModule(m_device.get(), "vert.spv"))
    , m_fragmentShaderModule(new VulkanShaderModule(m_device.get(), "frag.spv"))
    , m_pipeline(new VulkanPipeline(m_device.get(), m_swapchain.get()))
    , m_commandPool(new VulkanCommandPool(m_device.get()))
    , m_imageAvailableSemaphore(new VulkanSemaphore(m_device.get()))
    , m_renderFinishedSemaphore(new VulkanSemaphore(m_device.get()))
{
    m_pipeline->addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, m_vertexShaderModule.get());
    m_pipeline->addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, m_fragmentShaderModule.get());
    m_pipeline->create();

    const auto renderPass = m_swapchain->renderPass();
    const auto &framebuffers = m_swapchain->framebuffers();
    m_commandBuffers.reserve(framebuffers.size());
    const VkRect2D renderArea = {
        .offset = VkOffset2D { 0, 0 },
        .extent = VkExtent2D { m_swapchain->width(), m_swapchain->height() }
    };
    for (size_t i = 0; i < framebuffers.size(); ++i) {
        auto commandBuffer = std::make_unique<VulkanCommandBuffer>(m_device.get(), m_commandPool.get());
        commandBuffer->begin();
        commandBuffer->beginRenderPass(renderPass, framebuffers[i], renderArea);
        commandBuffer->bindPipeline(m_pipeline.get());
        commandBuffer->draw(3, 1, 0, 0);
        commandBuffer->endRenderPass();
        commandBuffer->end();
        m_commandBuffers.push_back(std::move(commandBuffer));
    }
}

VulkanRenderer::~VulkanRenderer() = default;

void VulkanRenderer::render() const
{
    uint32_t imageIndex = m_swapchain->acquireNextImage(m_imageAvailableSemaphore.get());

    const VkCommandBuffer commandBuffer = m_commandBuffers[imageIndex]->handle();
    const VkSemaphore imageAvailable = m_imageAvailableSemaphore->handle();
    const VkSemaphore renderFinished = m_renderFinishedSemaphore->handle();
    const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &imageAvailable,
        .pWaitDstStageMask = &waitStage,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &renderFinished
    };
    if (vkQueueSubmit(m_device->queue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit command");

    m_swapchain->queuePresent(imageIndex, m_renderFinishedSemaphore.get());
}

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
    std::unique_ptr<VulkanRenderer> m_renderer;
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
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, Demo::keyCallback);

    m_renderer.reset(new VulkanRenderer(m_window, width, height));
}

void Demo::terminate()
{
    m_renderer.reset();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Demo::renderLoop()
{
    while (!glfwWindowShouldClose(m_window)) {
        m_renderer->render();
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