
#include "real/resource/resource.hpp"
#include <real/resource/resource_shader.hpp>

namespace real {

const char *ShaderType_to_string(const ShaderType type) {
	constexpr const char *arr[] = {
		"COMPUTE", "VERTEX", "FRAGMENT", "VERTEX_FRAGMENT"};
	return arr[(int)type];
}

const char *ShaderFieldType_to_string(const ShaderFieldType type) {
	constexpr const char *arr[] = {
		"UNIFORM", "STORAGE_IMAGE", "PUSH_CONSTANT"
	};

	return arr[(int)type];
}

const char *ShaderDataType_to_string(const ShaderDataType type) {
	constexpr const char *arr[] = {
		"NONE", "FLOAT", "FLOAT2", "FLOAT3", "FLOAT4",
		"INT", "INT2", "INT3", "INT4"
	};

	return arr[(int)type];
}

ResourceShader::ResourceShader(
        Instance *_instance, std::vector<char> data,
		std::vector<ShaderField> fields, ShaderType _type,
		std::optional<Path> _path) 
	: Resource(_instance, _path), type(_type) {}

ResourceShader::~ResourceShader() = default;

ShaderField ShaderLayout::get_field(std::string str) {
	return field_map[str];
}

void ShaderLayout::add_field_entry(ShaderField field) {
	field_map.emplace(field.name, field);
	fields.push_back(field);
}

}

