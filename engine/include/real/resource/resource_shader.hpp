#ifndef REALLIB_RESOURCE_SHADER_HPP
#define REALLIB_RESOURCE_SHADER_HPP

#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/core/vec.hpp"
#include "real/resource/resource.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace real {

/**
 * @brief defines flags for the various shader stages that we have
 */
enum ShaderTypeFlag : u32 {
	ShaderTypeFlag_NONE 		= 0x0,
    ShaderTypeFlag_COMPUTE 		= 0x1,
    ShaderTypeFlag_VERTEX 		= 0x2,
    ShaderTypeFlag_FRAGMENT 	= 0x4,
	ShaderTypeFlag_GEOMETRY 	= 0x8,
};
using ShaderTypeFlags = u32;

/**
 * @brief what type of field this shader data is stored in
 */
enum class ShaderFieldType {
	UNIFORM,
	PUSH_CONSTANT
};

/**
 * @brief the type of data being stored in this shader.
 */
enum class ShaderDataType {
	NONE,
	FLOAT,
	FLOAT2,
	FLOAT3,
	FLOAT4,
	INT,
	INT2,
	INT3,
	INT4,
	FLOAT4x4,
	FLOAT3x3,
	FLOAT2x2,
	STRUCT,
	POINTER,
	SAMPLED_IMAGE,
	UNIFORM_BUFFER,
	STORAGE_IMAGE
};

REALLIB_EXPORT 
const char *ShaderType_to_string(const ShaderTypeFlag type);

REALLIB_EXPORT 
const char *ShaderFieldType_to_string(const ShaderFieldType type);

REALLIB_EXPORT 
const char *ShaderDataType_to_string(const ShaderDataType type);

/**
 * @brief defines a field in a shader that can be accessed.
 * stores data like the location of the data, what type of data it is, and
 * information about how that data is stored in the shader.
 */
struct ShaderField {
	ShaderTypeFlag stage;
    ShaderFieldType type;
	ShaderDataType data_type;

	// active if its an array
	bool is_array;
	int array_size;

    std::string name;
    size_t location;
	size_t offset;
};

/**
 * @brief manages an array of shader fields for fast lookup by name.
 */
struct REALLIB_EXPORT ShaderLayout {
public:
	ShaderField get_field(std::string str) const;
	void add_field_entry(ShaderField field);

	std::vector<ShaderField> fields;
	std::unordered_map<std::string, ShaderField> field_map;
};

class REALLIB_EXPORT ResourceShader : public Resource {
RL_OBJECT(ResourceShader, Resource)
EXPOSE_TO_EDITOR

public:
	struct InternalData {
		Vec3Int compute_threads {0,0,0};
	};

protected:
    ResourceShader(
        Instance *_instance, std::vector<char> data, 
		std::vector<ShaderField> fields, u32 _type,
		std::optional<Path> _path);

public:
    ~ResourceShader();

	const ShaderLayout &get_layout() const { return layout; }
	ShaderTypeFlags get_type() const { return type; }
	InternalData get_internal_data() const { return internal; }

protected:
	ShaderLayout layout;
	ShaderTypeFlags type;
	InternalData internal;
};

}

#endif
