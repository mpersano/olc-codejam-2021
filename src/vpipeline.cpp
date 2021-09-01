#include "vpipeline.h"

#include "vdevice.h"
#include "vpipelinelayout.h"
#include "vshadermodule.h"
#include "vswapchain.h"

#include <stdexcept>

namespace V {

PipelineBuilder::PipelineBuilder(const Device *device)
    : m_device(device)
{
}

PipelineBuilder &PipelineBuilder::addVertexInputBinding(uint32_t binding, uint32_t stride)
{
    VkVertexInputBindingDescription bindingDescription = {
        .binding = binding,
        .stride = stride,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };
    m_vertexInputBindings.push_back(bindingDescription);
    return *this;
}

PipelineBuilder &PipelineBuilder::addVertexInputAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset)
{
    VkVertexInputAttributeDescription attributeDescription = {
        .location = location,
        .binding = binding,
        .format = format,
        .offset = offset
    };
    m_vertexInputAttributes.push_back(attributeDescription);
    return *this;
}

PipelineBuilder &PipelineBuilder::setViewport(uint32_t width, uint32_t height)
{
    m_viewport = VkViewport {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(width),
        .height = static_cast<float>(height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    m_scissor = VkRect2D {
        .offset = VkOffset2D { 0, 0 },
        .extent = VkExtent2D { width, height },
    };
    return *this;
}

PipelineBuilder &PipelineBuilder::addShaderStage(VkShaderStageFlagBits stage, ShaderModule *module)
{
    VkPipelineShaderStageCreateInfo shaderStage = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = stage,
        .module = module->handle(),
        .pName = "main",
    };
    m_shaderStages.push_back(shaderStage);
    return *this;
}

std::unique_ptr<Pipeline> PipelineBuilder::create(const PipelineLayout *layout, VkRenderPass renderPass) const
{
    VkPipelineVertexInputStateCreateInfo vertexInputState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = static_cast<uint32_t>(m_vertexInputBindings.size()),
        .pVertexBindingDescriptions = m_vertexInputBindings.empty() ? nullptr : m_vertexInputBindings.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertexInputAttributes.size()),
        .pVertexAttributeDescriptions = m_vertexInputAttributes.empty() ? nullptr : m_vertexInputAttributes.data()
    };
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };
    VkPipelineViewportStateCreateInfo viewportState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &m_viewport,
        .scissorCount = 1,
        .pScissors = &m_scissor
    };
    VkPipelineRasterizationStateCreateInfo rasterizationState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f,
    };
    VkPipelineMultisampleStateCreateInfo multisampleState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };
    VkPipelineColorBlendStateCreateInfo colorBlendState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachmentState,
        .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
    };
    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast<uint32_t>(m_shaderStages.size()),
        .pStages = m_shaderStages.empty() ? nullptr : m_shaderStages.data(),
        .pVertexInputState = &vertexInputState,
        .pInputAssemblyState = &inputAssemblyState,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizationState,
        .pMultisampleState = &multisampleState,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &colorBlendState,
        .pDynamicState = nullptr,
        .layout = layout->handle(),
        .renderPass = renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };
    return std::make_unique<Pipeline>(m_device, graphicsPipelineCreateInfo);
}

Pipeline::Pipeline(const Device *device, const VkGraphicsPipelineCreateInfo &createInfo)
    : m_device(device)
{
    if (vkCreateGraphicsPipelines(m_device->device(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline");
}

Pipeline::~Pipeline()
{
    if (m_handle != VK_NULL_HANDLE)
        vkDestroyPipeline(m_device->device(), m_handle, nullptr);
}

} // namespace V
