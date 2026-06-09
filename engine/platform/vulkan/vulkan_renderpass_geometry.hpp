#ifndef REALLIB_VULKAN_RENDER_PASS_GEOMETRY_HPP
#define REALLIB_VULKAN_RENDER_PASS_GEOMETRY_HPP

#include "real/core/object.hpp"
#include "real/graphics/render_pass.hpp"
#include "real/graphics/render_pass_geometry.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_shader.hpp"
#include "vulkan_resource_image.hpp"
#include <cstddef>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace real {

class VulkanRenderPassGeometry : public RenderPassGeometry {
RL_OBJECT(VulkanRenderPassGeometry, RenderPassGeometry)

public:
	VulkanRenderPassGeometry(
		Instance *_instance, RenderPassGeometryInfo info,
		std::vector<ResourceHandle<ResourceShader>> _shaders,
		std::vector<RenderPassResource> _resources);

	~VulkanRenderPassGeometry();

public:
	void begin_pass(FrameContext context) final override;
	void end_pass(FrameContext context) final override;
	void set_variable(ShaderField field, char *data, size_t size) final override;
	virtual void draw_mesh(FrameContext context, ResourceHandle<ResourceMesh> mesh) final override;

private:
	void enable_depth(VkPipelineDepthStencilStateCreateInfo &depth);

private:
	VkPipeline pipeline;
	VkPipelineLayout layout;
	ShaderField addr_loc;

	VkDescriptorSet descriptor_set;
	VkDescriptorSetLayout descriptor_set_layout;

	// size_t color_image_index {0};
	// size_t depth_image_index {0};
	ResourceHandle<VulkanResourceImage> renderImage;
	std::optional<ResourceHandle<VulkanResourceImage>> depthImage;

	PushConstantBuffer push_constant_buffer;
};

};

#endif
