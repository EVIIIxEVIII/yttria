#include "yttria/backend/device.hpp"
#include "yttria/backend/image.hpp"
#include "yttria/backend/compute_pipeline.hpp"

#include <memory>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

struct InkImages {
    std::unique_ptr<Image> velocity;
    std::unique_ptr<Image> dye;
    std::unique_ptr<Image> dyeNext;
};

class InkSim {

public:
    InkSim(Device& device, VkDescriptorSetLayout globalSetLayout, const InkImages& inkImages);
    ~InkSim();

    void record(VkCommandBuffer cmd, VkDescriptorSet set);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline();

    Device& device_;
    const InkImages& inkImages_;
    std::unique_ptr<ComputePipeline> compPipeline_;
    VkPipelineLayout pipelineLayout_;
};


}
