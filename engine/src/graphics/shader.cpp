
#include <real/graphics/shader.hpp>

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

Shader::Shader(Instance *_instance)
	: Object(_instance) {}

}

