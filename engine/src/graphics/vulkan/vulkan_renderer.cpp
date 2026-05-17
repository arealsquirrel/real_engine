
#include "real/graphics/graphics.hpp"
#include "vulkan_backend.hpp"
#include <cassert>
#include <cmath>
#include <real/graphics/renderer.hpp>
#include <vulkan/vulkan_core.h>
#include "vulkan_util.hpp"

namespace real {

Renderer::Renderer(Instance *_instance, Window *_window)
    : window(_window), instance(_instance) {
    render_data = new RendererDataVulkan;
    RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    
    VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(window_backend->graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();

	for (int i = 0; i < VULKAN_FRAME_OVERLAP; i++) {
        /* COMMAND POOLS AND BUFFERS */
		VK_CHECK(vkCreateCommandPool(
            window_backend->device,
            &commandPoolInfo,
            nullptr, &rdata->frame_data[i].command_pool));
		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(
            rdata->frame_data[i].command_pool, 1);
		VK_CHECK(vkAllocateCommandBuffers(
            window_backend->device, &cmdAllocInfo,
             &rdata->frame_data[i].main_command_buffer));
	

        /* SYNC OBJECTS */
        VK_CHECK(vkCreateFence(
            window_backend->device,
            &fenceCreateInfo, nullptr,
            &rdata->frame_data[i].render_fence));
	
		VK_CHECK(vkCreateSemaphore(
            window_backend->device,
            &semaphoreCreateInfo,
            nullptr, &rdata->frame_data[i].swapchain_semaphores));
    }
}

Renderer::~Renderer() {
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 

    vkDeviceWaitIdle(window_backend->device);

    for (int i = 0; i < VULKAN_FRAME_OVERLAP; i++) {
        vkDestroyCommandPool(window_backend->device, rdata->frame_data[i].command_pool, nullptr);

	vkDestroyFence(window_backend->device, rdata->frame_data[i].render_fence, nullptr);
	vkDestroySemaphore(window_backend->device, rdata->frame_data[i].swapchain_semaphores, nullptr);
    
        rdata->frame_data[i].delete_queue.flush();
    }

    rdata->delete_queue.flush();
}

void Renderer::draw() {
    RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    FrameDataVulkan &frame = rdata->frame_data[frame_number % VULKAN_FRAME_OVERLAP];

    VK_CHECK(vkWaitForFences(
        window_backend->device, 1,
        &frame.render_fence, true, 1000000000));
	VK_CHECK(vkResetFences(
        window_backend->device, 1,
        &frame.render_fence));
	uint32_t swapchainImageIndex=0;
	VK_CHECK(vkAcquireNextImageKHR(
        window_backend->device,
        window_backend->swapchain,
        1000000000, frame.swapchain_semaphores,
        nullptr, &swapchainImageIndex));

    VkCommandBuffer cmd = frame.main_command_buffer;
	VK_CHECK(vkResetCommandBuffer(cmd, 0));
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    vkinit::transition_image(
        cmd, window_backend->swapchain_images[swapchainImageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    

    //make a clear-color from frame number. This will flash with a 120 frame period.
    VkClearColorValue clearValue;
    float flash = std::abs(std::sin(frame_number / 120.f));
    clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

    VkImageSubresourceRange clearRange = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
    vkCmdClearColorImage(
        cmd, window_backend->swapchain_images[swapchainImageIndex],
        VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

    vkinit::transition_image(
        cmd, window_backend->swapchain_images[swapchainImageIndex],
        VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    VK_CHECK(vkEndCommandBuffer(cmd));


    VkCommandBufferSubmitInfo cmdinfo = vkinit::command_buffer_submit_info(cmd);	
    VkSemaphoreSubmitInfo waitInfo = vkinit::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        frame.swapchain_semaphores);
    VkSemaphoreSubmitInfo signalInfo = vkinit::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                window_backend->render_semaphore[swapchainImageIndex]);	
    VkSubmitInfo2 submit = vkinit::submit_info(
        &cmdinfo,&signalInfo
        ,&waitInfo);	
    VK_CHECK(vkQueueSubmit2(
        window_backend->graphics_queue, 1,
        &submit, frame.render_fence));

    //prepare present
    // this will put the image we just rendered to into the visible window.
    // we want to wait on the _renderSemaphore for that, 
    // as its necessary that drawing commands have finished before the image is displayed to the user
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &window_backend->swapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pWaitSemaphores = &window_backend->render_semaphore[swapchainImageIndex];
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &swapchainImageIndex;

    VK_CHECK(vkQueuePresentKHR(window_backend->graphics_queue, &presentInfo));
    frame.delete_queue.flush();
    frame_number++;
}

}
