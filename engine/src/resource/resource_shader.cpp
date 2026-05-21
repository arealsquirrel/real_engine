
#include "real/resource/resource.hpp"
#include <real/resource/resource_shader.hpp>

namespace real {

ResourceShader::ResourceShader(
        Instance *_instance, Optional<Path> _path,
		std::vector<char> data, ShaderType _type) 
	: Resource(_instance, _path), type(_type) {}

ResourceShader::~ResourceShader() = default;

}

