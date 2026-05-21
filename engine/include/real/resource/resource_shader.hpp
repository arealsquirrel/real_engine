#ifndef REALLIB_RESOURCE_SHADER_HPP
#define REALLIB_RESOURCE_SHADER_HPP

#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource.hpp"
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
    ShaderFieldType type;
    std::string name;
    int location;
};

struct ShaderLoadStruct {
    ShaderType type;
};

typedef void* ShaderHandle;

/**
 * @brief the backend is entirly in the render folder of the API in use
 */
class ResourceShader : public Resource {
protected:
    ResourceShader(
        Instance *_instance, Optional<Path> _path,
		std::vector<char> data, ShaderType _type);

public:
    ~ResourceShader();

public:
    const ShaderType type;
};

}

#endif
