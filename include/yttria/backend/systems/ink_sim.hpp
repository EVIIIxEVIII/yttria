#include "yttria/backend/device.hpp"
#include "yttria/backend/graphics_pipeline.hpp"

#include <memory>
#include <vulkan/vulkan.h>

namespace yttria::backend {


class InkSim {

public:
    InkSim(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~InkSim();

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& device_;
    std::unique_ptr<GraphicsPipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;
};


}
