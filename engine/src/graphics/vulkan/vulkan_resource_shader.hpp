#ifndef REALLIB_VUKLAN_RESOURCE_SHADER_HPP
#define REALLIB_VUKLAN_RESOURCE_SHADER_HPP

#include "real/core/types.hpp"
#include "real/resource/resource_shader.hpp"
#include <vulkan/vulkan_core.h>

namespace real {

class VulkanRenderer;

class VulkanResourceShader : public ResourceShader {
public:
	VulkanResourceShader(
			Instance *_instance, std::vector<char> data,
			std::vector<ShaderField> fields, ShaderType _type);

	~VulkanResourceShader();

public:
    VkShaderModule module;
	Shared<VulkanRenderer> renderer;
};

}

#endif
