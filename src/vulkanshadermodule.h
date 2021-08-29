#pragma once

#include "noncopyable.h"
#include "vulkandevice.h"

#include <vulkan/vulkan.h>

namespace V {

class ShaderModule : private NonCopyable
{
public:
    explicit ShaderModule(const Device *device, const char *spvFilePath);
    ~ShaderModule();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkShaderModule handle() const { return m_handle; }

private:
    const Device *m_device;
    VkShaderModule m_handle = VK_NULL_HANDLE;
};

} // namespace V
