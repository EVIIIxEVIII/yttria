#pragma once
#include "yttria/backend/device.hpp"
#include "yttria/backend/buffer.hpp"

#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <memory>

namespace yttria::backend {

class Model {

public:
    struct Vertex {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

        bool operator==(const Vertex& other) const {
            return
                position == other.position &&
                color == other.color &&
                normal == other.normal &&
                uv == other.uv;
        }
    };

    struct Builder {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};

        void loadModel(const std::string &filepath);
    };

    Model(Device& device, const Builder &builder);
    ~Model();

    Model(const Model&) = delete;
    Model operator=(const Model&) = delete;

    static std::unique_ptr<Model> createModelFromFile(Device &device, const std::string &filepath);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);
    void createIndexBuffer(const std::vector<uint32_t> &indices);

    Device& device_;

    std::unique_ptr<Buffer> vertexBuffer_;
    uint32_t vertexCount_;

    bool hasIndexBuffer_ = false;
    std::unique_ptr<Buffer> indexBuffer_;
    uint32_t indexCount_;
};


}
