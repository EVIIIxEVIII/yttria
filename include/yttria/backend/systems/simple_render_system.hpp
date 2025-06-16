#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

#include "yttria/backend/graphics_pipeline.hpp"
#include "yttria/backend/device.hpp"
#include "yttria/backend/frame_info.hpp"

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
    std::unique_ptr<GraphicsPipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;
};

}

