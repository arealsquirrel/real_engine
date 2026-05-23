#ifndef REALLIB_RESOURCE_SHADER_HPP
#define REALLIB_RESOURCE_SHADER_HPP

#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource.hpp"
#include <functional>
#include <set>
#include <vector>

namespace real {

enum class ShaderType {
    COMPUTE,
    VERETEX,
    FRAGMENT
};

enum class ShaderFieldType {
    STORAGE_IMAGE,
    VECTOR_2,
    VECTOR_3,
    VECTOR_4
};

struct ShaderField {
    const ShaderFieldType type;
    const std::string name;
    const int location;
};


typedef void* ShaderHandle;

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
	const std::vector<ShaderField> fields;
    const ShaderType type;
};

}

#endif
