
#include <cassert>
#include <iostream>
#include <real/graphics/graphics.hpp>
#include <real/core/instance.hpp>
#include "vulkan_backend.hpp"
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>

namespace real {

static GraphicsBackendVulkan backend;

void Graphics::init_backend(const GraphicsInfo &info) {
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name("Example Vulkan Application")
		.request_validation_layers(true)
        // .enable_layer("VK_LAYER_MESA_overlay")
		.use_default_debug_messenger()
		.require_api_version(1, 3, 0)
		.build();

    vkb::Instance ins = inst_ret.value();
    backend.vkbInstance = ins;
    backend.instance = ins.instance;
    backend.debug_messenger = ins.debug_messenger;
}

GraphicsBackend Graphics::get_backend() {
    return &backend;
}

void Graphics::destroy_backend() {
    vkb::destroy_debug_utils_messenger(backend.instance, backend.debug_messenger);
    vkDestroyInstance(backend.instance, nullptr);
}

}
