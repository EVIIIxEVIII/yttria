#pragma once

#include "VulkanCamera.hpp"
#include "VulkanSceneObject.hpp"
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

namespace yttria::backend {

#define MAX_LIGHTS 10

struct PointLight {
    glm::vec4 position{}; // alpha ignored
    glm::vec4 color{}; // alpha is intensity
};

struct GlobalUbo {
    glm::mat4 projection{1.f};
    glm::mat4 view{1.f};
    glm::mat4 inverseView{1.f};
    glm::vec4 ambientLightColor{1.f, 1.f, 1.f, 0.02f}; // alpha is the intensity
    PointLight pointLights[MAX_LIGHTS];
    int numLights;
};

struct FrameInfo {
    int framIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera &camera;
    VkDescriptorSet globalDescriptorSet;
    SceneObject::Map &sceneObjects;
};

}
