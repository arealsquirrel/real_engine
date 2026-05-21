#ifndef REALLIB_VULKAN_DESCRIPTOR_ALLOCATOR_HPP
#define REALLIB_VULKAN_DESCRIPTOR_ALLOCATOR_HPP

#include <initializer_list>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace real {

struct DescriptorAllocator {
    struct PoolSizeRatio{
        VkDescriptorType type;
        float ratio;
    };

    VkDescriptorPool pool;

    void init_pool(VkDevice device, uint32_t maxSets, std::initializer_list<PoolSizeRatio> poolRatios);
    void clear_descriptors(VkDevice device);
    void destroy_pool(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};
    
}

#endif
