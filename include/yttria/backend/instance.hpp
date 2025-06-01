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

        VkInstance getInstance() { return instance_; }

    private:
        bool checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();
        void createInstance();

        VkInstance instance_;
};

}
