#pragma once

#include "noncopyable.h"
#include "vulkandevice.h"

#include <vulkan/vulkan.h>

class VulkanShaderModule : private NonCopyable
{
public:
    explicit VulkanShaderModule(const VulkanDevice *device, const char *spvFilePath);
    ~VulkanShaderModule();

    const VulkanDevice *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkShaderModule handle() const { return m_handle; }

private:
    const VulkanDevice *m_device;
    VkShaderModule m_handle = VK_NULL_HANDLE;
};
