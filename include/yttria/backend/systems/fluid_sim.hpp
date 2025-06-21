#pragma once
#include "yttria/backend/device.hpp"
#include "yttria/backend/frame_info.hpp"
#include "yttria/backend/image.hpp"
#include "yttria/backend/compute_pipeline.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

struct FluidContext {
    std::unique_ptr<Image> velocity;
    std::unique_ptr<Image> fluid;
    std::unique_ptr<Image> fluidNext;
    glm::uvec3             dim;
};

struct FluidSimPushConstants {
    float frameTime;
};

class FluidSim {

public:
    FluidSim(Device& device, VkDescriptorSetLayout globalSetLayout, const FluidContext& fluidContext);
    ~FluidSim();

    void record(FrameInfo& frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline();

    Device& device_;
    const FluidContext& fluidContext_;
    std::unique_ptr<ComputePipeline> compPipeline_;
    VkPipelineLayout pipelineLayout_;
};


}
