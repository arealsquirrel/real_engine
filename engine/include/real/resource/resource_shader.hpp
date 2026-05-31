#ifndef REALLIB_RESOURCE_SHADER_HPP
#define REALLIB_RESOURCE_SHADER_HPP

#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace real {

enum ShaderType : u32 {
	ShaderType_INFER 		= 0,
    ShaderType_COMPUTE 		= 1,
    ShaderType_VERTEX 		= 2,
    ShaderType_FRAGMENT 	= 4,
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

REALLIB_EXPORT 
const char *ShaderType_to_string(const ShaderType type);

REALLIB_EXPORT 
const char *ShaderFieldType_to_string(const ShaderFieldType type);

REALLIB_EXPORT 
const char *ShaderDataType_to_string(const ShaderDataType type);

struct REALLIB_EXPORT ShaderLayout {
public:
	ShaderField get_field(std::string str);
	void add_field_entry(ShaderField field);

	std::vector<ShaderField> fields;
	std::unordered_map<std::string, ShaderField> field_map;
};

/**
 * @brief the backend is entirly in the render folder of the API in use
 */
class REALLIB_EXPORT ResourceShader : public Resource {
RL_OBJECT(ResourceShader, Resource)
EXPOSE_TO_EDITOR

protected:
    ResourceShader(
        Game *_game, std::vector<char> data, 
		std::vector<ShaderField> fields, u32 _type,
		std::optional<Path> _path);

public:
    ~ResourceShader();

	ShaderLayout get_layout() const { return layout; }
	u32 get_type() const { return type; }

protected:
	ShaderLayout layout;
	u32 type;
};

}

#endif
