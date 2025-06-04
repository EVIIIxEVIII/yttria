#include "yttria/frontend/rendered.hpp"

namespace yttria::frontend {

Renderer::Renderer():
    window_(200, 300, "Renderer"),
    instance_(validationLayers_),
    surface_(instance_.instance(), window_.getGLFWWindow()),
    physicalDevice_(instance_.instance(), surface_.surface()),
    logicalDevice_(
        physicalDevice_.getPhysicalDevice(),
        physicalDevice_.getFamilyIndices(),
        deviceExtensions_,
        validationLayers_,
        true,
    )
{

}

}

