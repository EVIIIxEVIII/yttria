#pragma once

#include "yttria/backend/device.hpp"

#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

struct GraphicsPipelineConfigInfo {
    GraphicsPipelineConfigInfo(const GraphicsPipelineConfigInfo&) = delete;
    GraphicsPipelineConfigInfo& operator=(const GraphicsPipelineConfigInfo&) = delete;

    std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;

    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

struct ShaderInfo {
    std::optional<std::string> vertPath;
    std::optional<std::string> fragPath;
    std::optional<std::string> compPath;
};

class GraphicsPipeline {

public:
    GraphicsPipeline(
        Device &device,
        const std::string& vertPath,
        const std::string& fragPath,
        const GraphicsPipelineConfigInfo& configInfo
    );

    ~GraphicsPipeline();
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    static void defaultGraphicsPipelineConfigInfo(GraphicsPipelineConfigInfo& configInfo);

private:
    static std::vector<char> readFile(const std::string& filename);
    void createGraphicsPipeline(
        const std::string& vertPath,
        const std::string& fragPath,
        const GraphicsPipelineConfigInfo& configInfo
    );

    void createShaderModule(const std::vector<char>& code, VkShaderModule *shaderModule);
    std::vector<VkPipelineShaderStageCreateInfo> createShaders(ShaderInfo shaderInfo);

    Device& device_;
    VkPipeline pipeline_;
    VkShaderModule vertShaderModule_ = VK_NULL_HANDLE;
    VkShaderModule fragShaderModule_ = VK_NULL_HANDLE;
    VkShaderModule compShaderModule_ = VK_NULL_HANDLE;
};

}
