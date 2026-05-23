#ifndef REALLIB_VULKAN_BACKEND_HPP
#define REALLIB_VULKAN_BACKEND_HPP

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <VkBootstrap.h>

#include "vk_mem_alloc.h"

namespace real {

/**
 * @brief this is the backend per application.
 */

struct GraphicsBackendVulkan {
    vkb::Instance vkbInstance;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
};

}

#endif
