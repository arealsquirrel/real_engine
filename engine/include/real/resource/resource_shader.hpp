#ifndef REALLIB_RESOURCE_SHADER_HPP
#define REALLIB_RESOURCE_SHADER_HPP

#include <real/core/event.hpp>
#include "real/container/ref.hpp"
#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include "real/graphics/shader.hpp"
#include "real/resource/resource.hpp"
#include <vector>

namespace real {

class REALLIB_EXPORT ResourceShader : public Resource {
RL_OBJECT(ResourceShader, Resource)
EXPOSE_TO_EDITOR

public:
    ResourceShader(Instance *_instance, std::vector<char> shader_code);
    ~ResourceShader();

	const ShaderLayout &get_layout() const { return shader->get_layout(); }
	ShaderTypeFlags get_type() const { return shader->get_type(); }

public:
	UniquePointer<Shader> shader;
};

}

#endif
