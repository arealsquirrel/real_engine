#ifndef REALLIB_VUKLAN_RESOURCE_SHADER_HPP
#define REALLIB_VUKLAN_RESOURCE_SHADER_HPP

#include "real/core/game.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_shader.hpp"
#include <vulkan/vulkan_core.h>

namespace real {

class VulkanRenderer;

class VulkanResourceShader : public ResourceShader {
RL_OBJECT(VulkanResourceShader, ResourceShader)

public:
	VulkanResourceShader(
			Game *_game, std::vector<char> data,
			std::vector<ShaderField> fields, u32 _type,
			std::optional<Path> _path=std::nullopt);

	~VulkanResourceShader();

	// void write_to_field(ShaderField field, void *buffer, size_t size, FrameContext frame) override;

public:
    VkShaderModule module;
	Shared<VulkanRenderer> renderer;
	std::vector<VkDescriptorType> descriptor_types;
};

}

#endif
