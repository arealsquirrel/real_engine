#ifndef REALLIB_VUKLAN_RENDERPASS_COMPUTE
#define REALLIB_VUKLAN_RENDERPASS_COMPUTE

#include "real/graphics/render_pass_compute.hpp"
#include <vulkan/vulkan_core.h>

namespace real {

class VulkanRenderPassCompute : public RenderPassCompute {
public:
	VulkanRenderPassCompute(
		Instance *_instance,
        ResourceShader* shader,
		std::vector<ResourceHandle<ResourceImage>> inResources,
		std::vector<ResourceHandle<ResourceImage>> outResources);

	~VulkanRenderPassCompute();

private:
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkDescriptorSet descriptor_set;
    VkDescriptorSetLayout descriptor_set_layout;
};

}

#endif
