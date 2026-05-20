#ifndef REALLIB_VULKAN_DESCRIPTOR_ALLOCATOR_HPP
#define REALLIB_VULKAN_DESCRIPTOR_ALLOCATOR_HPP

#include <span>
#include <vulkan/vulkan.h>

namespace real {

struct DescriptorAllocator {
    struct PoolSizeRatio{
        VkDescriptorType type;
        float ratio;
    };

    VkDescriptorPool pool;

    void init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
    void clear_descriptors(VkDevice device);
    void destroy_pool(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};
    
}

#endif
