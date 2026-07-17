#ifndef REALLIB_VUKLAN_RESOURCE_SHADER_HPP
#define REALLIB_VUKLAN_RESOURCE_SHADER_HPP

#include "real/core/game.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/resource/resource_shader.hpp"
#include <vulkan/vulkan_core.h>
#include <spirv_reflect.h>

namespace real {

class VulkanRenderer;

class VulkanResourceShader : public ResourceShader {
RL_OBJECT(VulkanResourceShader, ResourceShader)

public:
	VulkanResourceShader(
			Instance *_instance, std::vector<char> data,
			std::vector<ShaderField> fields={},
			ShaderTypeFlags _type=ShaderTypeFlag_NONE);

	~VulkanResourceShader();

private:
	void serialize_shader(std::vector<char> data);
	void serialize_function_compute(SpvReflectEntryPoint fn);

public:
    VkShaderModule module;
	VulkanRenderer *renderer;
	std::vector<VkDescriptorType> descriptor_types;
};

}

#endif
