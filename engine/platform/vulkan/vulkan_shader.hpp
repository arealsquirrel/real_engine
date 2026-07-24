#ifndef REALLIB_VUKLAN_RESOURCE_SHADER_HPP
#define REALLIB_VUKLAN_RESOURCE_SHADER_HPP

#include "real/core/game.hpp"
#include "real/core/object.hpp"
#include "real/graphics/shader.hpp"
#include <vulkan/vulkan_core.h>
#include <spirv_reflect.h>

namespace real {

class VulkanRenderer;

class VulkanShader : public Shader {
RL_OBJECT(VulkanShader, Shader)

public:
	VulkanShader(
			Instance *_instance, std::vector<char> data,
			bool reflect = true,
			std::vector<ShaderField> fields={},
			ShaderTypeFlags _type=ShaderTypeFlag_NONE);

	~VulkanShader();

public:
    VkShaderModule module;
	VulkanRenderer *renderer;
	std::vector<VkDescriptorType> descriptor_types;
};

}

#endif
