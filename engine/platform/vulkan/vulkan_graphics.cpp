
#include <cassert>
#include <real/graphics/graphics.hpp>
#include "vulkan_backend.hpp"
#include <VkBootstrap.h>
#include <tracy/Tracy.hpp>
#include <vulkan/vulkan_core.h>
#include <real/resource/resource_shader.hpp>

namespace real {

static GraphicsBackendVulkan backend;

void Graphics::init_backend(const GraphicsInfo &info) {
	ZoneScoped

    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name(info.name)
		.request_validation_layers(true)
		.use_default_debug_messenger()
		.require_api_version(1, 3, 0)
		.enable_extension("VK_EXT_debug_utils")
		.build();

    vkb::Instance ins = inst_ret.value();
    backend.vkbInstance = ins;
    backend.instance = ins.instance;
    backend.debug_messenger = ins.debug_messenger;

	/*
	uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    std::cout << "Available Vulkan Layers:\n";
    for (const auto& layer : availableLayers) {
        std::cout << "\t- " << layer.layerName << ": " << layer.description << "\n";
    }
	*/
}

GraphicsBackend Graphics::get_backend() {
    return &backend;
}

void Graphics::destroy_backend() {
	ZoneScoped

    vkb::destroy_debug_utils_messenger(backend.instance, backend.debug_messenger);
    vkDestroyInstance(backend.instance, nullptr);
}

}
