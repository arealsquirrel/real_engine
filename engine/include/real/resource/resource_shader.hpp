#ifndef REALLIB_RESOURCE_SHADER_HPP
#define REALLIB_RESOURCE_SHADER_HPP

#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource.hpp"

namespace real {

typedef void* ShaderHandle;

/**
 * @brief the backend is entierly in the render folder of the API in use
 * 
 * the resource load struct is the renderer handle
 */
class ResourceShader : public Resource {
public:
    ResourceShader(Instance *_instance, Optional<Path> _path, WindowBackend _window, ShaderHandle _handle);
    ~ResourceShader();

    ShaderHandle get_handle() { return handle; }

private:
    ShaderHandle handle;
    WindowBackend window;
};

}

#endif