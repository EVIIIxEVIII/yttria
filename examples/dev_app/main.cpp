#include <iostream>
#include <memory>

#include "yttria/backend/window.hpp"

int main() {
    auto window = std::make_unique<yttria::backend::Window>(300, 400, "Dev app");

    while(!window->shouldClose()) {
        glfwPollEvents();


    }
}
