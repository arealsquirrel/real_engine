#ifndef REALLIB_RESOURCE_SHADER_HPP
#define REALLIB_RESOURCE_SHADER_HPP

#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource.hpp"
#include <initializer_list>
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
    std::initializer_list<ShaderField> fields;
    ShaderType type;
    WindowBackend window;
};

typedef void* ShaderHandle;

/**
 * @brief the backend is entierly in the render folder of the API in use
 */
class ResourceShader : public Resource {
public:
    ResourceShader(
        Instance *_instance, Optional<Path> _path,
        WindowBackend _window, ShaderHandle _handle,
        std::initializer_list<ShaderField> _fields, ShaderType _type);

    ~ResourceShader();

    ShaderHandle get_handle() { return handle; }

public:
    const std::vector<ShaderField> fields;
    const ShaderType type;

private:
    ShaderHandle handle;
    WindowBackend window;
};

}

#endif