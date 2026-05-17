
#include "real/graphics/window.hpp"
#include "real/resource/resource.hpp"
#include "vulkan_backend.hpp"
#include <optional>
#include <real/resource/resource_shader.hpp>
#include <vulkan/vulkan_core.h>

namespace real {

ResourceShader::ResourceShader(Instance *_instance, Optional<Path> _path, WindowBackend _window, ShaderHandle _handle) 
    : Resource(_instance, _path), window(_window), handle(_handle) {}

ResourceShader::~ResourceShader() {
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window;
    ShaderVulkan *shader = (ShaderVulkan*)handle;

    vkDestroyShaderModule(window_backend->device, shader->module, nullptr);

    delete shader;
}

template<> template<>
ResourceShader *ResourceSerializer<ResourceSerializerType::Disk>::load<ResourceShader>(
        Instance *instance,
        Optional<Path> path,
        const LoadStruct loadstruct) {

    WindowBackendVulkan *render_data = (WindowBackendVulkan*)loadstruct;

    if(path.has_value() == false) {
        instance->log.warn("resource shader vulkan std::optional path has not value");
        return nullptr;
    }

    std::ifstream file(path.value(), std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        instance->log.warn("std::ifstream failed to open file {}", path->c_str());
        return nullptr;
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);
    file.close();

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.codeSize = buffer.size() * sizeof(uint32_t);
    createInfo.pCode = buffer.data();

    ShaderVulkan *s = new ShaderVulkan;
    if (vkCreateShaderModule(render_data->device, &createInfo, nullptr, &s->module) != VK_SUCCESS) {
        instance->log.error("VkCreateShaderModule failed on shader {}", path->c_str());
        return nullptr;
    }

    ResourceShader *shader = new ResourceShader(instance, path, loadstruct, s);
    return shader;
}

}
