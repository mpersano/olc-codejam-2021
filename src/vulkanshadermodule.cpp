#include "vulkanshadermodule.h"

#include "util.h"

#include <stdexcept>

namespace V {

ShaderModule::ShaderModule(const Device *device, const char *spvFilePath)
    : m_device(device)
{
    const auto shaderCode = readFile(spvFilePath);

    VkShaderModuleCreateInfo shaderModuleCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = shaderCode.size(),
        .pCode = reinterpret_cast<const uint32_t *>(shaderCode.data())
    };

    if (vkCreateShaderModule(device->device(), &shaderModuleCreateInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module");
}

ShaderModule::~ShaderModule()
{
    if (m_handle != VK_NULL_HANDLE)
        vkDestroyShaderModule(m_device->device(), m_handle, nullptr);
}

} // namespace V
