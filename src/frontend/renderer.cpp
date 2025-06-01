#include "yttria/frontend/rendered.hpp"

namespace be = yttria::backend;

namespace yttria::frontend {

Renderer::Renderer():
    instance_(be::Instance()),
    window_(be::Window(200, 300, "Renderer"))
{}



}

