
#include "vulkan_resource_shader.hpp"
#include "fmt/color.h"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource.hpp"
#include "vulkan_renderer.hpp"
#include <cstdint>
#include <memory>
#include <real/resource/resource_shader.hpp>
#include <vulkan/vulkan_core.h>

namespace real {

VulkanResourceShader::VulkanResourceShader(
		Instance *_instance, std::vector<char> data, 
		std::vector<ShaderField> fields, ShaderType _type) 
	: ResourceShader(_instance, data, fields, _type), 
		renderer(std::dynamic_pointer_cast<VulkanRenderer>(_instance->renderer)) {
	
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.codeSize = data.size();
    createInfo.pCode = (uint32_t*)data.data();

    if (vkCreateShaderModule(renderer->device, &createInfo, nullptr, &module) != VK_SUCCESS) {
        instance->log.error("VkCreateShaderModule failed on shader womp womp");
    }
}

VulkanResourceShader::~VulkanResourceShader() {
    vkDestroyShaderModule(renderer->device, module, nullptr);
}

template<>
ResourceShader *Resource::load<ResourceSerializerType::Disk, ResourceShader>(
        Instance *instance,
        Optional<Path> path) {

    if(path.has_value() == false) {
        instance->log.warn("resource shader vulkan std::optional path does not have value");
        return nullptr;
    }

    std::ifstream file(path.value(), std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        instance->log.warn("std::ifstream failed to open file {}", path->c_str());
        return nullptr;
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);
    file.close();

    return (ResourceShader*)(new VulkanResourceShader(
				instance, buffer,
				{{ShaderFieldType::STORAGE_IMAGE, "image", 0}},
				ShaderType::COMPUTE));
}

}
