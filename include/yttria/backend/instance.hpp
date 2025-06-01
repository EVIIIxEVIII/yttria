#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

class Instance {
    public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

        Instance();
        VkInstance instance() { return instance_; }

    private:
        bool checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();
        void createInstance();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData
        );

        VkInstance instance_;
        const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        void hasGflwRequiredInstanceExtensions();
};

}
