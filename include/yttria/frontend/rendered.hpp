#pragma once
#include "yttria/backend/instance.hpp"
#include "yttria/backend/window.hpp"


namespace yttria::frontend {

class Renderer {
    public:
        Renderer();

    private:
        backend::Instance instance_;
        backend::Window window_;

};

}
