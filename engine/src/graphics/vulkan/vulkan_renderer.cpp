
#include "real/graphics/graphics.hpp"
#include "real/resource/resource_image.hpp"
#include "vulkan_backend.hpp"
#include <cassert>
#include <cmath>
#include <optional>
#include <real/graphics/renderer.hpp>
#include <vulkan/vulkan_core.h>
#include "vulkan_descriptor_allocator.hpp"
#include "vulkan_util.hpp"

namespace real {

Renderer::Renderer(Instance *_instance, Window *_window)
    : window(_window), instance(_instance) {
    render_data = new RendererDataVulkan;
    RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    
    VkCommandPoolCreateInfo commandPoolInfo = vkutil::command_pool_create_info(window_backend->graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkFenceCreateInfo fenceCreateInfo = vkutil::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkutil::semaphore_create_info();

	for (int i = 0; i < VULKAN_FRAME_OVERLAP; i++) {
        /* COMMAND POOLS AND BUFFERS */
		VK_CHECK(vkCreateCommandPool(
            window_backend->device,
            &commandPoolInfo,
            nullptr, &rdata->frame_data[i].command_pool));
		VkCommandBufferAllocateInfo cmdAllocInfo = vkutil::command_buffer_allocate_info(
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

    rdata->render_image = new ResourceImage(instance, std::nullopt, 500, 500, ColorFormat::RGB_FLOAT);
}

Renderer::~Renderer() {
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 

    vkDeviceWaitIdle(window_backend->device);

    delete rdata->render_image;

    for (int i = 0; i < VULKAN_FRAME_OVERLAP; i++) {
        vkDestroyCommandPool(window_backend->device, rdata->frame_data[i].command_pool, nullptr);

	    vkDestroyFence(window_backend->device, rdata->frame_data[i].render_fence, nullptr);
	    vkDestroySemaphore(window_backend->device, rdata->frame_data[i].swapchain_semaphores, nullptr);
    
        rdata->frame_data[i].delete_queue.flush();
    }

    rdata->delete_queue.flush();

    delete (RendererDataVulkan*)render_data;
}

FrameContext Renderer::start_frame() {
    RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    FrameDataVulkan *frame = &rdata->frame_data[frame_number % VULKAN_FRAME_OVERLAP];
    ImageVulkan *render_image_handle = (ImageVulkan*) rdata->render_image->get_handle();

    frame->draw_extent.width = render_image_handle->imageExtent.width;
    frame->draw_extent.height = render_image_handle->imageExtent.height;


    VK_CHECK(vkWaitForFences(
        window_backend->device, 1,
        &frame->render_fence, true, 1000000000));
	VK_CHECK(vkResetFences(
        window_backend->device, 1,
        &frame->render_fence));
	VK_CHECK(vkAcquireNextImageKHR(
        window_backend->device,
        window_backend->swapchain,
        1000000000, frame->swapchain_semaphores,
        nullptr, &frame->swapchain_index));

    VkCommandBuffer cmd = frame->main_command_buffer;
	VK_CHECK(vkResetCommandBuffer(cmd, 0));
	VkCommandBufferBeginInfo cmdBeginInfo = vkutil::command_buffer_begin_info(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    // transition our main draw image into general layout so we can write into it
	// we will overwrite it all so we dont care about what was the older layout
	vkutil::transition_image(cmd, render_image_handle->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    return frame;
}

void Renderer::end_frame(FrameContext context) {
    RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    FrameDataVulkan *frame = (FrameDataVulkan*)context;
    VkCommandBuffer cmd = frame->main_command_buffer;
    ImageVulkan *render_image_handle = (ImageVulkan*) rdata->render_image->get_handle();


	//transition the draw image and the swapchain image into their correct transfer layouts
	vkutil::transition_image(cmd, render_image_handle->image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vkutil::transition_image(cmd, window_backend->swapchain_images[frame->swapchain_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vkutil::copy_image_to_image(cmd, render_image_handle->image, window_backend->swapchain_images[frame->swapchain_index], frame->draw_extent, window_backend->swapchain_extent);
	vkutil::transition_image(cmd, window_backend->swapchain_images[frame->swapchain_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    VK_CHECK(vkEndCommandBuffer(cmd));
    
    VkCommandBufferSubmitInfo cmdinfo = vkutil::command_buffer_submit_info(cmd);	
    VkSemaphoreSubmitInfo waitInfo = vkutil::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        frame->swapchain_semaphores);
    VkSemaphoreSubmitInfo signalInfo = vkutil::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                window_backend->render_semaphore[frame->swapchain_index]);	
    VkSubmitInfo2 submit = vkutil::submit_info(
        &cmdinfo,&signalInfo
        ,&waitInfo);	
    VK_CHECK(vkQueueSubmit2(
        window_backend->graphics_queue, 1,
        &submit, frame->render_fence));

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &window_backend->swapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pWaitSemaphores = &window_backend->render_semaphore[frame->swapchain_index];
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &frame->swapchain_index;

    VK_CHECK(vkQueuePresentKHR(window_backend->graphics_queue, &presentInfo));
    frame->delete_queue.flush();
    frame_number++;
}

}
