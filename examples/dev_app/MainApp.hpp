#pragma once

#include "yttria/backend/window.hpp"
#include "yttria/backend/device.hpp"
#include "yttria/backend/renderer.hpp"
#include "yttria/backend/scene_object.hpp"
#include "yttria/backend/descriptors.hpp"
#include "yttria/backend/systems/fluid_sim.hpp"

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

    std::vector<float> initFluid(const FluidContext& fluidContext);
    std::vector<glm::vec4> initVelocity(const FluidContext& fluidContext);

    Window window{WIDTH, HEIGHT, "Vulkan App"};
    Device device{window};
    Renderer renderer{window, device};

    std::unique_ptr<DescriptorPool> globalPool{};
    SceneObject::Map sceneObjects;
};

}


