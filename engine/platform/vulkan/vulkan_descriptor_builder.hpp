#ifndef REALLIB_VUKLAN_DESCRIPTOR_BUILDER_HPP
#define REALLIB_VUKLAN_DESCRIPTOR_BUILDER_HPP

#include <vector>
#include <vulkan/vulkan_core.h>

namespace real {

struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void add_binding(uint32_t binding, VkDescriptorType type, size_t arr_size=1);
    void clear();
    VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
};

}

#endif
