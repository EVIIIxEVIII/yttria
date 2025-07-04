#include "MainApp.hpp"

#include <cstdlib>
#include <iostream>

int main() {
    Application::MainApp app{};

    try {
        app.run();
    } catch(const std::exception &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

