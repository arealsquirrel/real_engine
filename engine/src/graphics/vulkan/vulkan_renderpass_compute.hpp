#ifndef REALLIB_VUKLAN_RENDERPASS_COMPUTE
#define REALLIB_VUKLAN_RENDERPASS_COMPUTE

#include "real/core/object.hpp"
#include "real/resource/resource_shader.hpp"
#include "vulkan_resource_shader.hpp"

#include <vulkan/vulkan_core.h>
#include <real/graphics/render_pass_compute.hpp>

namespace real {

class VulkanRenderPassCompute : public RenderPassCompute {
RL_OBJECT(VulkanRenderPassCompute, RenderPassCompute)

public:
	VulkanRenderPassCompute(
		Game *_game,
        ResourceHandle<ResourceShader> _shader,
		std::vector<RenderPassResource> _resources);

	~VulkanRenderPassCompute();

public:
    void begin_pass(FrameContext context) override;
    void end_pass(FrameContext context) override;
	void set_variable(ShaderField field, char *data, size_t size) override;

private:
	PushConstantBuffer push_constant_buffer;
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkDescriptorSet descriptor_set;
    VkDescriptorSetLayout descriptor_set_layout;
};

}

#endif
