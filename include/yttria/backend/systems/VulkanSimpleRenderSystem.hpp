#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

#include "../VulkanPipeline.hpp"
#include "../VulkanDevice.hpp"
#include "../VulkanFrameInfo.hpp"

namespace yttria::backend {

class SimpleRenderSystem {

public:
    SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem operator=(const SimpleRenderSystem&) = delete;

    void renderSceneObjects(FrameInfo &frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& device_;
    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;
};

}

