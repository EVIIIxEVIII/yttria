#include "yttria/backend/device.hpp"
#include "yttria/backend/frame_info.hpp"
#include "yttria/backend/image.hpp"
#include "yttria/backend/compute_pipeline.hpp"

#include <memory>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

struct FluidImages {
    std::unique_ptr<Image> velocity;
    std::unique_ptr<Image> dye;
    std::unique_ptr<Image> dyeNext;
};

struct FluidSimPushConstants {
    float frameTime;
};

class FluidSim {

public:
    FluidSim(Device& device, VkDescriptorSetLayout globalSetLayout, const FluidImages& inkImages);
    ~FluidSim();

    void record(FrameInfo& frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline();

    Device& device_;
    const FluidImages& inkImages_;
    std::unique_ptr<ComputePipeline> compPipeline_;
    VkPipelineLayout pipelineLayout_;
};


}
