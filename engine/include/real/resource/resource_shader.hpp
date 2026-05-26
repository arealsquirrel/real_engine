#ifndef REALLIB_RESOURCE_SHADER_HPP
#define REALLIB_RESOURCE_SHADER_HPP

#include "real/core/core.hpp"
#include "real/core/instance.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace real {

enum class ShaderType {
	INFER,
    COMPUTE,
    VERTEX,
    FRAGMENT,
	VERTEX_FRAGMENT // it has both
};

enum class ShaderFieldType {
	UNIFORM,
	STORAGE_IMAGE,
	PUSH_CONSTANT
};

enum class ShaderDataType {
	NONE,
	FLOAT,
	FLOAT2,
	FLOAT3,
	FLOAT4,
	INT,
	INT2,
	INT3,
	INT4
};

struct ShaderField {
    ShaderFieldType type;
	ShaderDataType data;
    std::string name;
    size_t location;
	size_t offset;
};

const char *ShaderType_to_string(const ShaderType type);
const char *ShaderFieldType_to_string(const ShaderFieldType type);
const char *ShaderDataType_to_string(const ShaderDataType type);

struct ShaderLayout {
public:
	ShaderField get_field(std::string str);
	void add_field_entry(ShaderField field);

	std::vector<ShaderField> fields;
	std::unordered_map<std::string, ShaderField> field_map;
};

/**
 * @brief the backend is entirly in the render folder of the API in use
 */
class ResourceShader : public Resource {
RL_CLASS(ResourceShader)
EXPOSE_TO_EDITOR

protected:
    ResourceShader(
        Instance *_instance, std::vector<char> data, 
		std::vector<ShaderField> fields, ShaderType _type,
		std::optional<Path> _path);

public:
    ~ResourceShader();

	ShaderLayout get_layout() { return layout; }

protected:
	ShaderLayout layout;
	ShaderType type;
};

}

#endif
