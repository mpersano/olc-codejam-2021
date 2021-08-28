#pragma once

#include "noncopyable.h"

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanCommandPool : private NonCopyable
{
public:
    explicit VulkanCommandPool(VulkanDevice *device);
    ~VulkanCommandPool();

    VkCommandPool handle() const { return m_handle; }

private:
    VulkanDevice *m_device;
    VkCommandPool m_handle;
};
