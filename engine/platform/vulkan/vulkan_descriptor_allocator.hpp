#ifndef REALLIB_VULKAN_DESCRIPTOR_ALLOCATOR_HPP
#define REALLIB_VULKAN_DESCRIPTOR_ALLOCATOR_HPP

#include <deque>
#include <initializer_list>
#include <tuple>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace real {

struct DescriptorAllocator {

    struct PoolSizeRatio{
		VkDescriptorType type;
		float ratio;
    };

    VkDescriptorPool pool;

    void init_pool(VkDevice device, uint32_t maxSets, std::vector<PoolSizeRatio> poolRatios);
    void clear_descriptors(VkDevice device);
    void destroy_pool(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};

struct DescriptorAllocatorGrowable {
public:
	struct PoolSizeRatio {
		VkDescriptorType type;
		float ratio;
	};

	void init(VkDevice device, uint32_t initialSets, std::vector<PoolSizeRatio> poolRatios);
	void clear_pools(VkDevice device);
	void destroy_pools(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext = nullptr);
private:
	VkDescriptorPool get_pool(VkDevice device);
	VkDescriptorPool create_pool(
			VkDevice device, uint32_t setCount,
			std::vector<PoolSizeRatio> poolRatios);

	std::vector<PoolSizeRatio> ratios;
	std::vector<VkDescriptorPool> fullPools;
	std::vector<VkDescriptorPool> readyPools;
	uint32_t setsPerPool;
};

struct DescriptorWriter {
	using ImageArray = std::vector<std::tuple<VkImageView, VkSampler, VkImageLayout>>;

	std::deque<std::vector<VkDescriptorImageInfo>> image_arrays;
    std::deque<VkDescriptorImageInfo> imageInfos;
    std::deque<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkWriteDescriptorSet> writes;

    void write_image(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type);
	void write_image_array(int binding, ImageArray array, VkDescriptorType type);
    void write_buffer(int binding, VkBuffer buffer,size_t size, size_t offset,VkDescriptorType type); 

    void clear();
    void update_set(VkDevice device, VkDescriptorSet set);
};

}

#endif
