
#include "vulkan_resource_shader.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource.hpp"
#include "vulkan_renderer.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <real/resource/resource_shader.hpp>
#include <vulkan/vulkan_core.h>
#include <spirv_reflect.h>

#define CHECK_FLAG(x, n) ((x & n) != 0)

namespace real {

static ShaderDataType reflect_to_datatype(SpvReflectTypeDescription* type) {
	SpvReflectTypeFlags tf = type->type_flags;
	if(CHECK_FLAG(tf, SPV_REFLECT_TYPE_FLAG_VECTOR)) {
		if(CHECK_FLAG(tf, SPV_REFLECT_TYPE_FLAG_FLOAT)) {
			switch (type->traits.numeric.vector.component_count) {
			case(4): return ShaderDataType::FLOAT4;
			case(3): return ShaderDataType::FLOAT3;
			case(2): return ShaderDataType::FLOAT2;
			}
		}
	}

	return ShaderDataType::NONE;
}

VulkanResourceShader::VulkanResourceShader(
		Instance *_instance, std::vector<char> data, 
		std::vector<ShaderField> fields, ShaderType _type,
		std::optional<Path> _path) 
	: ResourceShader(_instance, data, fields, _type, _path), 
		renderer(std::dynamic_pointer_cast<VulkanRenderer>(_instance->renderer)) {
	
	SpvReflectShaderModule spvmodule;
	SpvReflectResult result = spvReflectCreateShaderModule(data.size(), (uint32_t*)data.data(), &spvmodule);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	if(CHECK_FLAG(spvmodule.shader_stage,SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT)) {
		type = ShaderType::COMPUTE;
	} else if (CHECK_FLAG(spvmodule.shader_stage,SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)) {
		type = ShaderType::VERTEX;
	} else if (CHECK_FLAG(spvmodule.shader_stage,SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT)) {
		type = ShaderType::FRAGMENT;
	} else if (
			CHECK_FLAG(spvmodule.shader_stage,SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT) &&
			CHECK_FLAG(spvmodule.shader_stage,SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)) {
		type = ShaderType::VERTEX_FRAGMENT;
	}

	/* ---------- DESCRIPTOR BINDINGS ---------- */
	uint32_t count = 0;
	result = spvReflectEnumerateDescriptorBindings(&spvmodule, &count, NULL);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);
	std::vector<SpvReflectDescriptorBinding*> bindings(count);
	result = spvReflectEnumerateDescriptorBindings(&spvmodule, &count, bindings.data());
	assert(result == SPV_REFLECT_RESULT_SUCCESS);
	for(size_t i = 0; i < count; i++) {
		SpvReflectDescriptorBinding *var = bindings[i];
		SpvReflectDescriptorType t = var->descriptor_type;
		descriptor_types.push_back(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		layout.add_field_entry({ShaderFieldType::STORAGE_IMAGE, ShaderDataType::NONE, var->name, i, 0});
	}

	/* ---------- PUSH CONSTANTS ---------- */	
	result = spvReflectEnumeratePushConstantBlocks(&spvmodule, &count, NULL);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);
	std::vector<SpvReflectBlockVariable*> push_constant(count);
	result = spvReflectEnumeratePushConstantBlocks(&spvmodule, &count, push_constant.data());
	assert(result == SPV_REFLECT_RESULT_SUCCESS);
	for (size_t i = 0; i < count; i++) {
		SpvReflectBlockVariable *block = push_constant[i];
		size_t offset = 0;
		
		for (size_t j = 0; j < block->member_count; j++) {
			layout.add_field_entry({
					ShaderFieldType::PUSH_CONSTANT, 
					reflect_to_datatype(block->members[j].type_description),
					block->members[j].name, i, block->members[j].offset});
		}
	}

	if(count >= 1) {
		// pushConstants = (char*)malloc(128);
	}

	spvReflectDestroyShaderModule(&spvmodule);

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.codeSize = data.size();
    createInfo.pCode = (uint32_t*)data.data();

    if (vkCreateShaderModule(renderer->device, &createInfo, nullptr, &module) != VK_SUCCESS) {
        RL_LOG_ERROR("VkCreateShaderModule failed on shader womp womp");
    }
}

VulkanResourceShader::~VulkanResourceShader() {
	// free(pushConstants);
    vkDestroyShaderModule(renderer->device, module, nullptr);
}

template<>
ResourceShader *Resource::load<ResourceSerializerType::Disk, ResourceShader>(
        Instance *instance,
        Optional<Path> path) {

    if(path.has_value() == false) {
        RL_LOG_WARN("resource shader vulkan std::optional path does not have value");
        return nullptr;
    }

    std::ifstream file(path.value(), std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        RL_LOG_WARN("std::ifstream failed to open file {}", path->c_str());
        return nullptr;
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);
    file.close();

	ShaderType type;

    return (ResourceShader*)(new VulkanResourceShader(
				instance, buffer, {}, ShaderType::INFER, path));
}

}
