
#include <cassert>
#include <real/graphics/graphics.hpp>
#include <real/core/instance.hpp>
#include "real/graphics/render_pass_compute.hpp"
#include "real/graphics/window.hpp"
#include "vulkan_backend.hpp"
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
		.request_validation_layers(info.debug)
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

Shared<Window> Graphics::create_window(Instance *instance, const WindowInfo &info) {
	return std::make_shared<Window>(instance, info);
}

Shared<Renderer> Graphics::create_renderer(Instance *instance, Shared<Window> window) {
	return std::make_shared<VulkanRenderer>(instance, window);
}

Shared<RenderPassCompute> Graphics::create_render_pass_compute(
	Instance *_instance, ResourceHandle<ResourceShader> shader,
	std::vector<ResourceHandle<ResourceImage>> _inResources,
	std::vector<ResourceHandle<ResourceImage>> _outResources) {

	return std::make_shared<VulkanRenderPassCompute>(_instance, shader, _inResources, _outResources);
}

}
