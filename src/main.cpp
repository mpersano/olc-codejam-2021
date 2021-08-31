#include "vbuffer.h"
#include "vcommandbuffer.h"
#include "vcommandpool.h"
#include "vdescriptorpool.h"
#include "vdescriptorset.h"
#include "vdescriptorsetlayout.h"
#include "vdevice.h"
#include "vfence.h"
#include "vmemory.h"
#include "vpipeline.h"
#include "vpipelinelayout.h"
#include "vsemaphore.h"
#include "vshadermodule.h"
#include "vsurface.h"
#include "vswapchain.h"

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
    std::unique_ptr<V::Device> m_device;
    std::unique_ptr<V::Surface> m_surface;
    std::unique_ptr<V::Swapchain> m_swapchain;
    std::unique_ptr<V::ShaderModule> m_vertexShaderModule;
    std::unique_ptr<V::ShaderModule> m_fragmentShaderModule;
    std::unique_ptr<V::PipelineLayout> m_pipelineLayout;
    std::unique_ptr<V::Pipeline> m_pipeline;
    std::unique_ptr<V::CommandPool> m_commandPool;
    std::unique_ptr<V::Semaphore> m_imageAvailableSemaphore;
    std::unique_ptr<V::Semaphore> m_renderFinishedSemaphore;
    std::unique_ptr<V::Memory> m_memory;
    std::unique_ptr<V::Buffer> m_positionBuffer;
    std::unique_ptr<V::Buffer> m_colorBuffer;
    std::unique_ptr<V::DescriptorSetLayout> m_descriptorSetLayout;
    std::unique_ptr<V::DescriptorPool> m_descriptorPool;
    std::unique_ptr<V::DescriptorSet> m_descriptorSet;
    std::vector<std::unique_ptr<V::CommandBuffer>> m_commandBuffers;
    std::vector<std::unique_ptr<V::Fence>> m_frameFences;
};

VulkanRenderer::VulkanRenderer(GLFWwindow *window, int width, int height)
    : m_window(window)
    , m_device(new V::Device)
    , m_surface(m_device->createSurface(window))
    , m_swapchain(m_surface->createSwapchain(width, height, 3))
    , m_vertexShaderModule(m_device->createShaderModule("vert.spv"))
    , m_fragmentShaderModule(m_device->createShaderModule("frag.spv"))
    , m_commandPool(m_device->createCommandPool())
    , m_imageAvailableSemaphore(m_device->createSemaphore())
    , m_renderFinishedSemaphore(m_device->createSemaphore())
    , m_memory(m_device->allocateMemory(1024))
    , m_positionBuffer(m_device->createBuffer(512))
    , m_colorBuffer(m_device->createBuffer(512))
{
    m_positionBuffer->bindMemory(m_memory.get(), 0);
    m_colorBuffer->bindMemory(m_memory.get(), 512);

    {
        float *buffer = m_memory->map<float>();

        float *positions = buffer;
        // clang-format off
        positions[ 0] =  0.0; positions[ 1]  = -0.5; positions[ 2] = 0.0; positions[ 3] = 1.0;
        positions[ 4] =  0.5; positions[ 5]  =  0.5; positions[ 6] = 0.0; positions[ 7] = 1.0;
        positions[ 8] = -0.5; positions[ 9]  =  0.5; positions[10] = 0.0; positions[11] = 1.0;
        // clang-format on

        float *colors = &buffer[512 / sizeof(float)];
        // clang-format off
        colors[ 0] = 1.0; colors[ 1]  = 0.0; colors[ 2] = 0.0; colors[ 3] = 1.0;
        colors[ 4] = 1.0; colors[ 5]  = 1.0; colors[ 6] = 0.0; colors[ 7] = 1.0;
        colors[ 8] = 1.0; colors[ 9]  = 0.0; colors[10] = 1.0; colors[11] = 1.0;
        // clang-format on

        m_memory->unmap();
    }

    m_descriptorSetLayout = m_device->descriptorSetLayoutBuilder()
                                    .addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                                    .addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                                    .create();

    m_descriptorPool = m_device->descriptorPoolBuilder()
                               .add(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2)
                               .create();

    m_descriptorSet = m_descriptorPool->allocateDescriptorSet(m_descriptorSetLayout.get());
    m_descriptorSet->writeBuffer(0, m_positionBuffer.get());
    m_descriptorSet->writeBuffer(1, m_colorBuffer.get());

    m_pipelineLayout = m_device->pipelineLayoutBuilder().addSetLayout(m_descriptorSetLayout.get()).create();

    m_pipeline = m_device->pipelineBuilder()
                         .setViewport(m_swapchain->width(), m_swapchain->height())
                         .addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, m_vertexShaderModule.get())
                         .addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, m_fragmentShaderModule.get())
                         .create(m_pipelineLayout.get(), m_swapchain->renderPass());

    const auto backbufferCount = m_swapchain->backbufferCount();

    const auto renderPass = m_swapchain->renderPass();
    const auto &framebuffers = m_swapchain->framebuffers();
    m_commandBuffers.reserve(backbufferCount);
    const VkRect2D renderArea = {
        .offset = VkOffset2D { 0, 0 },
        .extent = VkExtent2D { m_swapchain->width(), m_swapchain->height() }
    };
    for (size_t i = 0; i < backbufferCount; ++i) {
        auto commandBuffer = m_commandPool->allocateCommandBuffer();
        commandBuffer->begin();
        commandBuffer->beginRenderPass(renderPass, framebuffers[i], renderArea);
        commandBuffer->bindPipeline(m_pipeline.get());
        commandBuffer->bindDescriptorSet(m_pipelineLayout.get(), m_descriptorSet.get());
        commandBuffer->draw(3, 1, 0, 0);
        commandBuffer->endRenderPass();
        commandBuffer->end();
        m_commandBuffers.push_back(std::move(commandBuffer));
    }

    m_frameFences.reserve(backbufferCount);
    for (size_t i = 0; i < backbufferCount; ++i)
        m_frameFences.push_back(m_device->createFence(true));
}

VulkanRenderer::~VulkanRenderer()
{
    for (auto &fence : m_frameFences)
        fence->wait();
}

void VulkanRenderer::render() const
{
    uint32_t imageIndex = m_swapchain->acquireNextImage(m_imageAvailableSemaphore.get());

    m_frameFences[imageIndex]->wait();
    m_frameFences[imageIndex]->reset();

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
    if (vkQueueSubmit(m_device->queue(), 1, &submitInfo, m_frameFences[imageIndex]->handle()) != VK_SUCCESS)
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
