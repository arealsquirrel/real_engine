#ifndef REALLIB_VUKLAN_RENDERPASS_COMPUTE
#define REALLIB_VUKLAN_RENDERPASS_COMPUTE

#include "real/core/object.hpp"
#include "real/resource/resource_shader.hpp"

#include <vulkan/vulkan_core.h>
#include <real/graphics/renderpass_compute.hpp>

namespace real {

class VulkanRenderPassCompute : public RenderPassCompute {
RL_OBJECT(VulkanRenderPassCompute, RenderPassCompute)

public:
	VulkanRenderPassCompute(
		Instance *_instance,
        ResourceHandle<ResourceShader> _shader,
        std::vector<RenderPassResource> _resources);

	~VulkanRenderPassCompute();

public:
    void begin_pass() final override;
    void end_pass() final override;
	void set_variable(ShaderField field, char *data, size_t size) final override;
	void set_variable_array(ShaderField field, char *data, size_t size) final override;
    void bind_descriptors() final override;
    void dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) final override;

private:
	PushConstantBuffer push_constant_buffer;
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkDescriptorSet descriptor_set;
    VkDescriptorSetLayout descriptor_set_layout;
};

}

#endif
