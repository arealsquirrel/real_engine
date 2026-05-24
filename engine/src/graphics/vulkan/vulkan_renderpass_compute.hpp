#ifndef REALLIB_VUKLAN_RENDERPASS_COMPUTE
#define REALLIB_VUKLAN_RENDERPASS_COMPUTE

#include "real/resource/resource_shader.hpp"

#include <vulkan/vulkan_core.h>
#include <real/graphics/render_pass_compute.hpp>

namespace real {

class VulkanRenderPassCompute : public RenderPassCompute {
public:
	VulkanRenderPassCompute(
		Instance *_instance,
        ResourceHandle<ResourceShader> _shader,
		std::vector<ResourceHandle<ResourceImage>> _inResources,
		std::vector<ResourceHandle<ResourceImage>> _outResources);

	~VulkanRenderPassCompute();

public:
    void begin_pass(FrameContext context) override;
    void end_pass(FrameContext context) override;

private:
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkDescriptorSet descriptor_set;
    VkDescriptorSetLayout descriptor_set_layout;
};

}

#endif
