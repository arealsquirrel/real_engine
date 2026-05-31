
#include <cassert>
#include <real/graphics/graphics.hpp>
#include "real/core/game.hpp"
#include "real/graphics/render_pass_geometry.hpp"
#include "real/graphics/render_pass_compute.hpp"
#include "real/graphics/window.hpp"
#include "vulkan_backend.hpp"
#include "vulkan_renderpass_geometry.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_renderpass_compute.hpp"
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>
#include <real/resource/resource_shader.hpp>

namespace real {

static GraphicsBackendVulkan backend;

void Graphics::init_backend(const GraphicsInfo &info) {
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
}

GraphicsBackend Graphics::get_backend() {
    return &backend;
}

void Graphics::destroy_backend() {
    vkb::destroy_debug_utils_messenger(backend.instance, backend.debug_messenger);
    vkDestroyInstance(backend.instance, nullptr);
}

Unique<Window> Graphics::create_window(Game *game, const WindowInfo &info) {
	return std::make_unique<Window>(game, info);
}

Unique<Renderer> Graphics::create_renderer(Game *game, Shared<Window> window) {
	return std::make_unique<VulkanRenderer>(game, window);
}

Unique<RenderPassCompute> Graphics::create_render_pass_compute(
	Game *game, ResourceHandle<ResourceShader> shader,
	std::vector<RenderPassResource> resources) {

	return std::make_unique<VulkanRenderPassCompute>(game, shader, resources);
}

Unique<RenderPassGeometry> Graphics::create_render_pass_geometry(
		Game *game, RenderPassGeometryInfo info,
		std::vector<ResourceHandle<ResourceShader>> shaders,
		std::vector<RenderPassResource> resources) {
	
	return std::make_unique<VulkanRenderPassGeometry>(game, info, shaders, resources);
}

}
