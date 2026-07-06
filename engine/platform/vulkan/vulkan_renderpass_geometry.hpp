#ifndef REALLIB_VULKAN_RENDER_PASS_GEOMETRY_HPP
#define REALLIB_VULKAN_RENDER_PASS_GEOMETRY_HPP

#include "real/core/object.hpp"
#include "real/graphics/renderpass.hpp"
#include "real/graphics/renderpass_geometry.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_shader.hpp"
#include "vulkan_descriptor_allocator.hpp"
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
	void begin_pass(Framebuffer *framebuffer, bool clear_depth=true) final override;
	void end_pass() final override;

	void set_variable(ShaderField field, char *data, size_t size) final override;
	void set_variable_array(ShaderField field, char *data, size_t size) final override;
	
	void bind_descriptors() final override;
	void draw_mesh(ResourceMesh *mesh, ResourceMesh::Mesh submesh) final override;
	void draw_mesh(ResourceMesh *mesh) final override;
	void draw_indexed(ResourceMesh *mesh, u32 indices, u32 instances, u32 start_index) final override;
	void draw(ResourceMesh *mesh, 
			u32 vertex_count, u32 instance_count,
			u32 first_vertex, u32 first_instance) final override;

private:
	VkPipelineDepthStencilStateCreateInfo create_depth(RenderPassGeometryInfo info);
	VkPipelineInputAssemblyStateCreateInfo create_input_assembly(RenderPassGeometryInfo info);
	VkPipelineRasterizationStateCreateInfo create_rasterizer(RenderPassGeometryInfo info);
	VkPipelineMultisampleStateCreateInfo create_multisample_control(RenderPassGeometryInfo info);

private:
	VkPipeline pipeline;
	VkPipelineLayout layout;
	ShaderField addr_loc;

	VkDescriptorSet descriptor_set;
	VkDescriptorSetLayout descriptor_set_layout;

	PushConstantBuffer push_constant_buffer;
	DescriptorWriter writer;
};

};

#endif
