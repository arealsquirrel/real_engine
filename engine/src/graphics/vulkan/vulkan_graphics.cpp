
#include <cassert>
#include <memory>
#include <real/graphics/graphics.hpp>
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/render_pass_geometry.hpp"
#include "real/graphics/render_pass_compute.hpp"
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

Unique<Window> Graphics::create_window(Instance *instance, const WindowInfo &info) {
	return std::make_unique<Window>(instance, info);
}

Unique<Renderer> Graphics::create_renderer(Instance *instance, Shared<Window> window) {
	return std::make_unique<VulkanRenderer>(instance, window);
}

Unique<RenderPassCompute> Graphics::create_render_pass_compute(
	Instance *instance, ResourceHandle<ResourceShader> shader,
	std::vector<RenderPassResource> resources) {

	return std::make_unique<VulkanRenderPassCompute>(instance, shader, resources);
}

Unique<RenderPassGeometry> Graphics::create_render_pass_geometry(
		Instance *instance, RenderPassGeometryInfo info,
		std::vector<ResourceHandle<ResourceShader>> shaders,
		std::vector<RenderPassResource> resources) {
	
	return std::make_unique<VulkanRenderPassGeometry>(instance, info, shaders, resources);
}

Unique<ResourceMesh> Graphics::create_resource_mesh(
		Instance *instance, std::vector<uint32_t> indices,
		char *data, size_t size) {

	return std::make_unique<VulkanResourceMesh>(instance, indices, data, size);
}

Unique<ResourceImage> Graphics::create_resource_image(
			Instance *instance, u32 width, u32 height,
			ColorFormat cformat, ImageFormat iformat, void *data, int mips) {
	
	return std::make_unique<VulkanResourceImage>(instance, width, height, cformat, iformat, data, mips);
}

}
