#ifndef REALLIB_RESOURCE_SHADER_HPP
#define REALLIB_RESOURCE_SHADER_HPP

#include "real/core/instance.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource.hpp"
#include <vector>

namespace real {

enum class ShaderType {
	INFER,
    COMPUTE,
    VERTEX,
    FRAGMENT,
	VERTEX_FRAGMENT // it has both
};

struct ShaderField {
    // const ShaderFieldType type;
    const std::string name;
    const int location;
};

// const char *ShaderFieldType_to_string(const ShaderFieldType type);
const char *ShaderType_to_string(const ShaderType type);


/**
 * @brief the backend is entirly in the render folder of the API in use
 */
class ResourceShader : public Resource {
protected:
    ResourceShader(
        Instance *_instance, std::vector<char> data, 
		std::vector<ShaderField> fields, ShaderType _type);

public:
    ~ResourceShader();

public:
	std::vector<ShaderField> fields;
	ShaderType type;
};

}

#endif
