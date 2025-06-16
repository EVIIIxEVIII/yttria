#include "yttria/backend/systems/simple_render_system.hpp"
#include "yttria/backend/graphics_pipeline.hpp"
#include "yttria/backend/scene_object.hpp"

#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/constants.hpp>

namespace yttria::backend {

struct SimplePushConstantData {
    alignas(16) glm::mat4 modelMatrix{1.f};
};

SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout):
    device_(device)
{
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
    vkDestroyPipelineLayout(device_.device(), pipelineLayout_, nullptr);
}

void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(device_.device(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create a pipeline layout");
    }
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

    GraphicsPipelineConfigInfo pipelineConfig{};
    GraphicsPipeline::defaultGraphicsPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipeline_ = std::make_unique<GraphicsPipeline>(
        device_,
        "/home/alderson/Projects/Mine/yttria/src/shaders/vert.spv",
        "/home/alderson/Projects/Mine/yttria/src/shaders/frag.spv",
        pipelineConfig
    );
}

void SimpleRenderSystem::renderSceneObjects(
    FrameInfo &frameInfo
) {
    pipeline_->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout_,
        0, 1,
        &frameInfo.globalDescriptorSet,
        0, nullptr
    );

    for (auto& kv: frameInfo.sceneObjects) {
        auto& obj = kv.second;
        if (obj.model == nullptr) continue;

        SimplePushConstantData push{};
        push.modelMatrix = obj.transform.mat4();

        vkCmdPushConstants(
            frameInfo.commandBuffer,
            pipelineLayout_,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push
        );

        obj.model->bind(frameInfo.commandBuffer);
        obj.model->draw(frameInfo.commandBuffer);
    }
}


}
