#include "MainApp.hpp"

#include "yttria/backend/systems/simple_render_system.hpp"
#include "yttria/backend/systems/ink_sim.hpp"

#include "yttria/backend/movement_controller.hpp"
#include "yttria/backend/image.hpp"
#include "yttria/backend/linear_clamp_sampler.hpp"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_float2x2.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/constants.hpp>
#include <chrono>

namespace Application {


MainApp::MainApp() {
    globalPool = DescriptorPool::Builder(device)
        .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, SwapChain::MAX_FRAMES_IN_FLIGHT * 3)
        .build();

    loadSceneObjects();
}

MainApp::~MainApp() {}

void MainApp::run() {
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<Buffer>(
            device,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            device.getProperties().limits.minUniformBufferOffsetAlignment
        );

        uboBuffers[i]->map();
    }

    auto linearClampSampler = std::make_unique<LinearClampSampler>(device);
    auto velocityImage = std::make_unique<Image>(
        device,
        256, 256, 256,
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_IMAGE_LAYOUT_GENERAL
    );

    auto currentDye = std::make_unique<Image>(
        device,
        256, 256, 256,
        VK_FORMAT_R16G16_SFLOAT, // the green channel is treated as an alpha one
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_IMAGE_LAYOUT_GENERAL
    );

    auto nextDye = std::make_unique<Image>(
        device,
        256, 256, 256,
        VK_FORMAT_R16_SFLOAT,
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_IMAGE_LAYOUT_GENERAL
    );

    auto globalSetLayout =
        DescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL)

            .addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
            .addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
            .addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
            .addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        auto velocityImageInfo = velocityImage->descriptorInfo(nullptr);
        auto nextDyeInfo = nextDye->descriptorInfo(nullptr);

        auto currentDyeInfoNoSampler = currentDye->descriptorInfo(nullptr);
        auto currentDyeInfo = currentDye->descriptorInfo(linearClampSampler->sampler());

        DescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .writeImage(1, &velocityImageInfo)
            .writeImage(2, &currentDyeInfoNoSampler)
            .writeImage(3, &nextDyeInfo)
            .writeImage(4, &currentDyeInfo)
            .build(globalDescriptorSets[i]);
    }

    SimpleRenderSystem simpleRenderSystem{
        device,
        renderer.getSwapChainRenderPass(),
        globalSetLayout->getDescriptorSetLayout()
    };

    InkImages inkImages{
        std::move(velocityImage),
        std::move(currentDye),
        std::move(nextDye)
    };

    InkSim inkSim (
        device,
        globalSetLayout->getDescriptorSetLayout(),
        inkImages
    );

    Camera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewerObject = SceneObject::createSceneObject();
    viewerObject.transform.translation.z = -2.5;

    MovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!window.shouldClose()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        cameraController.moveInPlaneXZ(window.getGLFWWindow(), frameTime, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = renderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(90.f), aspect, 0.1f, 100.f);

        if (auto commandBuffer = renderer.beginFrame()) {
            int frameIndex = renderer.getFrameIndex();
            FrameInfo frameInfo{
                frameIndex,
                frameTime,
                commandBuffer,
                camera,
                globalDescriptorSets[static_cast<size_t>(frameIndex)],
                sceneObjects
            };

            inkSim.record(commandBuffer, frameInfo.globalDescriptorSet);
            GlobalUbo ubo{};
            ubo.projection = camera.getProjection();
            ubo.view = camera.getView();
            ubo.inverseView = camera.getInverseView();
            uboBuffers[static_cast<size_t>(frameIndex)]->writeToBuffer((void*)&ubo);
            uboBuffers[static_cast<size_t>(frameIndex)]->flush();

            renderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderSceneObjects(frameInfo);
            renderer.endSwapChainRenderPass(commandBuffer);
            renderer.endFrame();
        }
    }

    vkDeviceWaitIdle(device.device());
};

void MainApp::loadSceneObjects() {
    std::shared_ptr<Model> floor = Model::createModelFromFile(device, "/home/alderson/Projects/Mine/yttria/examples/dev_app/models/quad.obj");
    auto floorObj = SceneObject::createSceneObject();
    floorObj.model = floor;
    floorObj.transform.translation = {0.f, 0.1f, 0.f};
    floorObj.transform.scale = 3.f;
    sceneObjects.emplace(floorObj.getId(), std::move(floorObj));

    std::vector<glm::vec3> lightColors{
      {1.f, .1f, .1f},
      {.1f, .1f, 1.f},
      {.1f, 1.f, .1f},
      {1.f, 1.f, .1f},
      {.1f, 1.f, 1.f},
      {1.f, 1.f, 1.f}
    };

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto pointLight = SceneObject::makePointLight(0.4f);
        pointLight.color = lightColors[i];
        auto rotateLight = glm::rotate(
            glm::mat4(1.f),
            (static_cast<float>(i) * glm::two_pi<float>()) / static_cast<float>(lightColors.size()),
            { 0.f, -1.f, 0.f}
        );

        pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-2.f, -3.f, -1.f, 1.f));
        sceneObjects.emplace(pointLight.getId(), std::move(pointLight));
    }

}

}

