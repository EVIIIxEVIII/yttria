#include "yttria/frontend/rendered.hpp"

namespace yttria::frontend {

Renderer::Renderer():
    window_(200, 300, "Renderer"),
    instance_(validationLayers_),
    surface_(instance_.get(), window_.GLFWWindow()),
    physicalDevice_(instance_.get(), surface_.get()),
    logicalDevice_(
        physicalDevice_.get(),
        physicalDevice_.FamilyIndices(),
        deviceExtensions_,
        validationLayers_,
        true
    ),
    commandPool_(logicalDevice_.get(), physicalDevice_.FamilyIndices())
{

}

}

