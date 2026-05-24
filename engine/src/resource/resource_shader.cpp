
#include "real/resource/resource.hpp"
#include <real/resource/resource_shader.hpp>

namespace real {

const char *ShaderType_to_string(const ShaderType type) {
	constexpr const char *arr[] = {
		"COMPUTE", "VERTEX", "FRAGMENT", "VERTEX_FRAGMENT"};
	return arr[(int)type];
}

ResourceShader::ResourceShader(
        Instance *_instance, std::vector<char> data,
		std::vector<ShaderField> fields, ShaderType _type) 
	: Resource(_instance), type(_type) {}

ResourceShader::~ResourceShader() = default;

}

