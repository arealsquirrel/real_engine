#ifndef REALLIB_VULKAN_RENDER_PASS_GEOMETRY_HPP
#define REALLIB_VULKAN_RENDER_PASS_GEOMETRY_HPP

#include "real/graphics/render_pass.hpp"
#include "real/graphics/render_pass_geometry.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_shader.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_resource_image.hpp"
#include <cstddef>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace real {

class VulkanRenderPassGeometry : public RenderPassGeometry {
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

private:
	VkPipeline pipeline;
	VkPipelineLayout layout;

	size_t color_image_index {0};

	PushConstantBuffer push_constant_buffer;
};

};

#endif
