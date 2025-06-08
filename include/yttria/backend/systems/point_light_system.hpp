#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

#include "yttria/backend/pipeline.hpp"
#include "yttria/backend/device.hpp"
#include "yttria/backend/frame_info.hpp"

namespace yttria::backend {

class PointLightSystem {

public:
    PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~PointLightSystem();

    PointLightSystem(const PointLightSystem&) = delete;
    PointLightSystem operator=(const PointLightSystem&) = delete;

    void update(FrameInfo &frameInfo, GlobalUbo &ubo);
    void render(FrameInfo &frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& device_;
    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;
};

}

