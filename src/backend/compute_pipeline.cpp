#include "yttria/backend/compute_pipeline.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

ComputePipeline::ComputePipeline(
    Device& device,
    const std::string& compPath,
    VkPipelineLayout pipelineLayout
): device_{device} {

    auto compShaderCode = readFile(compPath);

    createShaderModule(compShaderCode, &compShaderModule_);

    VkPipelineShaderStageCreateInfo compStageCreateInfo {};
    compStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    compStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    compStageCreateInfo.module = compShaderModule_;
    compStageCreateInfo.pName = "main";
    compStageCreateInfo.flags = 0;
    compStageCreateInfo.pNext = nullptr;
    compStageCreateInfo.pSpecializationInfo = nullptr;

    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.stage = compStageCreateInfo;

    if (vkCreateComputePipelines(device_.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create a compute pipeline!");
    }
}

ComputePipeline::~ComputePipeline() {
    vkDestroyShaderModule(device_.device(), compShaderModule_, nullptr);
    vkDestroyPipeline(device_.device(), computePipeline_, nullptr);
}

void ComputePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule *shaderModule) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device_.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module");
    }
}

std::vector<char> ComputePipeline::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Failed with file: "<< filename << "\n";
        throw std::runtime_error("failed to open file!");
    }

    std::streamsize fileSize = (std::streamsize) file.tellg();
    std::vector<char> buffer((size_t)fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}


}
