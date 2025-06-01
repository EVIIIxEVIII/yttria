#include "yttria/frontend/rendered.hpp"

namespace be = yttria::backend;

namespace yttria::frontend {

Renderer::Renderer():
    window_(be::Window(200, 300, "Renderer")),
    instance_(be::Instance()),
    surface_(instance_.instance(), window_.getGLFWWindow()),
    physicalDevice_(instance_.instance(), surface_.surface())
{

}

}

