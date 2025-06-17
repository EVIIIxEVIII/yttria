#include <yttria/backend/device.hpp>

namespace yttria::backend {

class ComputePipeline {
public:
    ComputePipeline(
        Device& device,
        const std::string& compPath,
        VkPipelineLayout pipelineLayout
    );
    ~ComputePipeline();

    VkPipeline pipeline() { return computePipeline_; }
private:
    Device& device_;
    VkPipeline computePipeline_;
    VkShaderModule compShaderModule_;

    std::vector<char> readFile(const std::string& filename);
    void createShaderModule(const std::vector<char>& code, VkShaderModule *shaderModule);
};

}
