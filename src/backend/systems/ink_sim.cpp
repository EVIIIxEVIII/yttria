#include "yttria/backend/systems/ink_sim.hpp"

#include <stdexcept>
#include <assert.h>

namespace yttria::backend {


InkSim::InkSim(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout): device_{device}
{
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

InkSim::~InkSim() {
    vkDestroyPipelineLayout(device_.device(), pipelineLayout_, nullptr);
}

void InkSim::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");


}

void InkSim::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(device_.device(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create a pipeline layout");
    }
}

}
