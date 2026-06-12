
#include "fmt/base.h"
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include "real/debug/timer.hpp"
#include "real/resource/resource.hpp"
#include <real/resource/resource_shader.hpp>

namespace real {

const char *ShaderType_to_string(const ShaderTypeFlags type) {
	RL_LOG_WARN("Function not implemented");

	return "null";
}

const char *ShaderFieldType_to_string(const ShaderFieldType type) {
	constexpr const char *arr[] = {
		"UNIFORM", "PUSH_CONSTANT"
	};

	return arr[(int)type];
}

const char *ShaderDataType_to_string(const ShaderDataType type) {
	constexpr const char *arr[] = {
		"NONE", "FLOAT", "FLOAT2", "FLOAT3", "FLOAT4",
		"INT", "INT2", "INT3", "INT4", 	"FLOAT4x4",
	"FLOAT3x3", "FLOAT2x2", "STRUCT", "POINTER", "SAMPLED_IMAGE",
	"UNIFORM_BUFFER", "STORAGE_IMAGE"
	};

	return arr[(int)type];
}

ResourceShader::ResourceShader(
        Instance *_instance, std::vector<char> data,
		std::vector<ShaderField> fields, u32 _type) 
	: Resource(_instance), type(_type) {}

ResourceShader::~ResourceShader() = default;

ShaderField ShaderLayout::get_field(std::string str) const {
	RL_INSTRUMENT_FUNCTION
	
	if(field_map.find(str) == field_map.end()) {
		RL_LOG_WARN("Field {} could not be found in shader layout", str);
	}

	return field_map.at(str);
}

void ShaderLayout::add_field_entry(ShaderField field) {
	field_map.emplace(field.name, field);
	fields.push_back(field);
}

}

