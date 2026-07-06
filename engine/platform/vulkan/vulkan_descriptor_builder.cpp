
#include "vulkan_descriptor_builder.hpp"
#include "vulkan_util.hpp"

namespace real {

void DescriptorLayoutBuilder::add_binding(uint32_t binding, VkDescriptorType type, size_t arr_size) {
    VkDescriptorSetLayoutBinding newbind {};
    newbind.binding = binding;
    newbind.descriptorCount = arr_size;
    newbind.descriptorType = type;

    bindings.push_back(newbind);
}

void DescriptorLayoutBuilder::clear() {
    bindings.clear();
}

VkDescriptorSetLayout DescriptorLayoutBuilder::build(
		VkDevice device, VkShaderStageFlags shaderStages,
		void* pNext, VkDescriptorSetLayoutCreateFlags flags) {

    for (auto& b : bindings) {
        b.stageFlags |= shaderStages;
		if(b.descriptorCount == 0) // OH YEA
			b.descriptorCount = 1;
    }

    VkDescriptorSetLayoutCreateInfo info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    info.pNext = pNext;

    info.pBindings = bindings.data();
    info.bindingCount = (uint32_t)bindings.size();
    info.flags = flags;

    VkDescriptorSetLayout set;
    VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

    return set;
}

}

