#pragma once

#include "yttria/backend/Window.hpp"
#include "yttria/backend/VulkanDevice.hpp"
#include "yttria/backend/VulkanRenderer.hpp"
#include "yttria/backend/VulkanSceneObject.hpp"
#include "yttria/backend/VulkanDescriptors.hpp"

using namespace yttria::backend;

namespace Application {

class MainApp {

public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    MainApp();
    ~MainApp();

    MainApp(const MainApp&) = delete;
    MainApp operator=(const MainApp&) = delete;

    void run();

private:
    void loadSceneObjects();

    Window window{WIDTH, HEIGHT, "Vulkan App"};
    Device device{window};
    Renderer renderer{window, device};

    std::unique_ptr<DescriptorPool> globalPool{};
    SceneObject::Map sceneObjects;
};

}


