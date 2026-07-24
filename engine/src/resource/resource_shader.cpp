
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/resource/resource.hpp"
#include <real/resource/resource_shader.hpp>
#include <tracy/Tracy.hpp>

namespace real {

ResourceShader::ResourceShader(Instance *_instance, std::vector<char> shader_code) 
	: Resource(_instance) {

	shader = Shader::create(instance, shader_code);
}

ResourceShader::~ResourceShader() = default;

ShaderField ShaderLayout::get_field(std::string str) const {
	ZoneScoped
	
	if(field_map.find(str) == field_map.end()) {
		RL_LOG_WARN("Field {} could not be found in shader layout", str);
	}

	return field_map.at(str);
}

void ShaderLayout::add_field_entry(ShaderField field) {
	field_map.emplace(field.name, field);
	fields.push_back(field);
}

template<>
ResourceHandle<ResourceShader> ResourceDatabase::load_resource_disk(Path path, std::string name) {
	ZoneScoped

    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        RL_LOG_WARN("std::ifstream failed to open file {}", path.c_str());
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);
    file.close();

	ResourceShader *shader = instance->engine_allocator.allocate_object<ResourceShader>(instance, buffer);
	return register_resource(shader, name, UUID(), path);
}

}

