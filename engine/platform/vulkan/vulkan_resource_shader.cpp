
#include "vulkan_resource_shader.hpp"
#include "real/core/core.hpp"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include "real/container/vec.hpp"
#include "real/resource/resource.hpp"
#include "vulkan_renderer.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <real/resource/resource_shader.hpp>
#include <string>
#include <tracy/Tracy.hpp>
#include <vulkan/vulkan_core.h>
#include <spirv_reflect.h>

namespace real {

static ShaderDataType reflect_to_datatype(SpvReflectTypeDescription* type) {
	SpvReflectTypeFlags tf = type->type_flags;
	if(CHECK_FLAG(tf, SPV_REFLECT_TYPE_FLAG_STRUCT)) {
		return ShaderDataType::STRUCT;
	}

	if(CHECK_FLAG(tf, SPV_REFLECT_TYPE_FLAG_VECTOR)) {
		if(CHECK_FLAG(tf, SPV_REFLECT_TYPE_FLAG_FLOAT)) {
			switch (type->traits.numeric.vector.component_count) {
			case(4): return ShaderDataType::FLOAT4;
			case(3): return ShaderDataType::FLOAT3;
			case(2): return ShaderDataType::FLOAT2;
			}
		}

		if(CHECK_FLAG(tf, SPV_REFLECT_TYPE_FLAG_INT)) {
			switch (type->traits.numeric.vector.component_count) {
			case(4): return ShaderDataType::INT4;
			case(3): return ShaderDataType::INT3;
			case(2): return ShaderDataType::INT2;
			}
		}
	}

	if(CHECK_FLAG(tf, SPV_REFLECT_TYPE_FLAG_MATRIX)) {
		if(CHECK_FLAG(tf, SPV_REFLECT_TYPE_FLAG_FLOAT)) {
			switch (type->traits.numeric.matrix.column_count) {
			case 4: return ShaderDataType::FLOAT4x4;
			}
		}
	}

	if(CHECK_FLAG(tf, SPV_REFLECT_TYPE_FLAG_FLOAT)) {
		return ShaderDataType::FLOAT;
	}

	if(CHECK_FLAG(tf, SPV_REFLECT_TYPE_FLAG_REF)) {
		return ShaderDataType::POINTER;
	}

	return ShaderDataType::NONE;
}

VulkanResourceShader::VulkanResourceShader(
		Instance *_instance, std::vector<char> data, 
		std::vector<ShaderField> fields, u32 _type) 
	: ResourceShader(_instance, data, fields, _type) {

	ZoneScoped
			
	renderer = (VulkanRenderer*)instance->renderer.get();

	serialize_shader(data);

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
	ZoneScoped
    vkDestroyShaderModule(renderer->device, module, nullptr);
}

void VulkanResourceShader::serialize_shader(std::vector<char> data) {
	ZoneScoped
	SpvReflectShaderModule spvmodule;
	SpvReflectResult result = spvReflectCreateShaderModule(data.size(), (uint32_t*)data.data(), &spvmodule);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	for (size_t i = 0; i < spvmodule.entry_point_count; i++) {
		SpvReflectEntryPoint fn = spvmodule.entry_points[i];

		switch (fn.shader_stage) {
		case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT: {
			type |= ShaderTypeFlag_COMPUTE;
			serialize_function_compute(fn);
			break;
		}
		case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT: {
			type |= ShaderTypeFlag_VERTEX;
			break;
		}
		case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT: {
			type |= ShaderTypeFlag_FRAGMENT;
			break;
		}
		case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT: {
			type |= ShaderTypeFlag_GEOMETRY;
			break;
		}
		default:
			RL_LOG_ERROR("could not resolve shader stage {} {}", (u32)fn.shader_stage, fn.name);
		}
	}

	uint32_t count = 0;
	result = spvReflectEnumeratePushConstantBlocks(&spvmodule, &count, NULL);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);
	std::vector<SpvReflectBlockVariable*> push_constant(count);
	result = spvReflectEnumeratePushConstantBlocks(&spvmodule, &count, push_constant.data());
	assert(result == SPV_REFLECT_RESULT_SUCCESS);
	if(count > 1) RL_LOG_WARN("we dont support more than one push constant block per shader");
	for (size_t i = 0; i < count; i++) {
		SpvReflectBlockVariable *block = push_constant[i];
		for (size_t j = 0; j < block->member_count; j++) {
			auto member = block->members[j];
			RL_LOG_TRACE(" 		variable {}", member.name);
			ShaderField field;
			field.stage = ShaderTypeFlag_NONE;
			field.type = ShaderFieldType::PUSH_CONSTANT;
			field.data_type = reflect_to_datatype(member.type_description);
			field.name = member.name;
			field.offset = member.offset;
			field.location = 0;

			field.is_array = false;
			field.array_size = 0;

			layout.add_field_entry(field);
		}
	}

	count = 0;
	result = spvReflectEnumerateDescriptorBindings(&spvmodule, &count, NULL);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);
	std::vector<SpvReflectDescriptorBinding*> bindings(count);
	result = spvReflectEnumerateDescriptorBindings(&spvmodule, &count, bindings.data());
	assert(result == SPV_REFLECT_RESULT_SUCCESS);
	for(size_t i = 0; i < count; i++) {
		SpvReflectDescriptorBinding *var = bindings[i];
		SpvReflectDescriptorType t = var->descriptor_type;
		ShaderField field;
		field.stage = ShaderTypeFlag_NONE;
		field.type = ShaderFieldType::UNIFORM;
		
		field.name = var->name;
		field.offset = 0;
		field.location = var->binding;

		RL_LOG_TRACE(" 		variable {} type {}", var->name, (u32)t);

		if(CHECK_FLAG(var->type_description->type_flags, SPV_REFLECT_TYPE_FLAG_ARRAY)) {
			if(var->array.dims_count > 1)
				RL_LOG_WARN("we dont have suppport for multi dimensional arrays in uniform buffers");
			field.is_array = true;
			field.array_size = var->array.dims[0];
		} else {
			field.is_array = false;
			field.array_size = 1;
		}

		if(t == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
			descriptor_types.push_back(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
			field.data_type = ShaderDataType::STORAGE_IMAGE;
		} else if (t == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
			descriptor_types.push_back(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
			field.data_type = ShaderDataType::SAMPLED_IMAGE;
		} else if (t == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
			descriptor_types.push_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			field.data_type = ShaderDataType::UNIFORM_BUFFER;
		} else {
			field.data_type = reflect_to_datatype(var->type_description);
		}

		layout.add_field_entry(field);
	}

	spvReflectDestroyShaderModule(&spvmodule);
}

void VulkanResourceShader::serialize_function_compute(SpvReflectEntryPoint fn) {
	internal.compute_threads = Vec3Int {
		(int)fn.local_size.x,
		(int)fn.local_size.y,
		(int)fn.local_size.z,
	};
}

template<>
ResourceHandle<ResourceShader> ResourceDatabase::load_resource_disk(Path path, std::string name) {

	ZoneScoped

    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        RL_LOG_WARN("std::ifstream failed to open file {}", path.c_str());
        // return nullptr;
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);
    file.close();

    auto shader = (ResourceShader*)(new VulkanResourceShader(instance, buffer, {}, ShaderTypeFlag_NONE));
	return register_resource(shader , name, UUID(), path);
}

}
