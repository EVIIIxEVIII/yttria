#include "yttria/backend/systems/point_light_system.hpp"
#include "yttria/backend/pipeline.hpp"

#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/constants.hpp>

namespace yttria::backend {

struct PointLightPushConstant {
    glm::vec4 position{};
    glm::vec4 color{};
    float radius;
};

PointLightSystem::PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout):
    device_(device)
{
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

PointLightSystem::~PointLightSystem() {
    vkDestroyPipelineLayout(device_.device(), pipelineLayout_, nullptr);
}

void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PointLightPushConstant);

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

void PointLightSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.attributeDescriptions.clear();
    pipelineConfig.bindingDescriptions.clear();

    ShaderInfo shaderInfo {
        "src/shaders/point_light.vert.spv",
        "src/shaders/point_light.frag.spv",
        "",
    };


    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipeline_ = std::make_unique<Pipeline>(
        device_,
        shaderInfo,
        pipelineConfig
    );
}

void PointLightSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo) {
    auto rotateLight = glm::rotate(
        glm::mat4(1.f),
        // frameInfo.frameTime,
        0.f,
        { 0.f, -1.f, 0.f}
    );

    int lightIndex = 0;
    for (auto& kv: frameInfo.sceneObjects) {
        auto& obj = kv.second;
        if (obj.pointLight == nullptr) continue;

        assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");

        obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.f));

        ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.f);
        ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);

        lightIndex += 1;
    }

    ubo.numLights = lightIndex;
}

void PointLightSystem::render(FrameInfo &frameInfo) {
    pipeline_->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout_,
        0, 1,
        &frameInfo.globalDescriptorSet,
        0, nullptr);

    for (auto& kv: frameInfo.sceneObjects) {
        auto& obj = kv.second;
        if (obj.pointLight == nullptr) continue;

        PointLightPushConstant push{};
        push.position = glm::vec4(obj.transform.translation, 1.f);
        push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
        push.radius = obj.transform.scale;

        vkCmdPushConstants(
            frameInfo.commandBuffer,
            pipelineLayout_,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(PointLightPushConstant),
            &push
        );

        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
}

}
