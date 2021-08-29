#pragma once

#include "noncopyable.h"
#include "vulkandevice.h"

#include <vulkan/vulkan.h>

#include <memory>

class VulkanCommandBuffer;

class VulkanCommandPool : private NonCopyable
{
public:
    explicit VulkanCommandPool(const VulkanDevice *device);
    ~VulkanCommandPool();

    const VulkanDevice *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkCommandPool handle() const { return m_handle; }

    std::unique_ptr<VulkanCommandBuffer> allocateCommandBuffer() const;

private:
    const VulkanDevice *m_device;
    VkCommandPool m_handle;
};
