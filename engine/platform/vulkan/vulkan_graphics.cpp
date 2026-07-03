
#include <cassert>
#include <iostream>
#include <memory>
#include <real/graphics/graphics.hpp>
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/debug/instrumentation.hpp"
#include "real/debug/timer.hpp"
#include "real/graphics/renderpass_geometry.hpp"
#include "real/graphics/renderpass_compute.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_mesh.hpp"
#include "vulkan_backend.hpp"
#include "vulkan_renderpass_geometry.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_renderpass_compute.hpp"
#include "vulkan_resource_image.hpp"
#include "vulkan_resource_mesh.hpp"
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>
#include <real/resource/resource_shader.hpp>

namespace real {

static GraphicsBackendVulkan backend;

void Graphics::init_backend(const GraphicsInfo &info) {
	RL_INSTRUMENT_FUNCTION;

    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name(info.name)
		.request_validation_layers(true)
		.use_default_debug_messenger()
		.require_api_version(1, 3, 0)
		.build();

    vkb::Instance ins = inst_ret.value();
    backend.vkbInstance = ins;
    backend.instance = ins.instance;
    backend.debug_messenger = ins.debug_messenger;

	uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    std::cout << "Available Vulkan Layers:\n";
    for (const auto& layer : availableLayers) {
        std::cout << "\t- " << layer.layerName << ": " << layer.description << "\n";
    }
}

GraphicsBackend Graphics::get_backend() {
    return &backend;
}

void Graphics::destroy_backend() {
	RL_INSTRUMENT_FUNCTION
    vkb::destroy_debug_utils_messenger(backend.instance, backend.debug_messenger);
    vkDestroyInstance(backend.instance, nullptr);
}

/*
Unique<RenderPassCompute> Graphics::create(
	Instance *instance, ResourceHandle<ResourceShader> shader,
	std::vector<RenderPassResource> _resources) {

	return std::make_unique<VulkanRenderPassCompute>(instance, shader, _resources);
}
*/

Unique<RenderPassGeometry> Graphics::create_render_pass_geometry(
		Instance *instance, RenderPassGeometryInfo info,
		std::vector<ResourceHandle<ResourceShader>> shaders,
		std::vector<RenderPassResource> _resources) {
	
	return std::make_unique<VulkanRenderPassGeometry>(instance, info, shaders, _resources);
}

}
