#include "vse.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

VseApp *vse_app_create(VseAppConfig vse_app_config)
{
    vse_info("STARTING VSE APPLICATION");

    VseApp vse_app;
    vse_app.window = vse_window_create(vse_app_config);
    vse_app.vk_instance = vse_instance_create(vse_app_config);
    vse_surface_set(&vse_app);

    vse_logger_physical_devices(vse_app.vk_instance);
    vse_app.vk_physical_device = vse_device_pick(vse_app.vk_instance, vse_app.vk_surface);
    vse_app.vk_device = vse_device_create(&vse_app);
    vse_app.vk_swapchain = vse_swapchain_create(&vse_app);
    vse_app.swapchain_image_views = vse_swapchain_create_image_views(vse_app);
    
    vse_renderpass_create(&vse_app);
    vse_pipeline_create(&vse_app);
    vse_framebuffer_create(&vse_app);
    vse_command_pool_create(&vse_app);
    vse_command_buffer_create(&vse_app);
    vse_syncobj_create(&vse_app);

    VseApp *p_vse_app = malloc(sizeof(VseApp));
    memcpy(p_vse_app, &vse_app, sizeof(VseApp));
    return p_vse_app;
}

// TO BE CHANGED TO FUNCTION POINTER IN THE APP CONFIG
void draw_frame(VseApp *vse_app) {
    vkWaitForFences(vse_app->vk_device, 1, &vse_app->fence_inflight, VK_TRUE, UINT64_MAX);
    vkResetFences(vse_app->vk_device, 1, &vse_app->fence_inflight);

    uint32_t image_index;
    vkAcquireNextImageKHR(vse_app->vk_device, vse_app->vk_swapchain, UINT64_MAX, vse_app->semaphore_image_available, VK_NULL_HANDLE, &image_index);

    vkResetCommandBuffer(vse_app->command_buffer, 0);
    vse_command_buffer_record(*vse_app, vse_app->command_buffer, image_index);

    VkSemaphore wait_semaphores[] = { vse_app->semaphore_image_available };
    VkSemaphore signal_semaphores[] = { vse_app->semaphore_render_finished };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,  
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = wait_semaphores,
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &vse_app->command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signal_semaphores,
    };

    VkResult queue_submit_result = vkQueueSubmit(vse_app->vk_graphics_queue, 1, &submit_info, vse_app->fence_inflight);
    if(queue_submit_result != VK_SUCCESS) {
        vse_err("Failed to submit queue.");
        exit(EXIT_FAILURE);
    }

    VkSwapchainKHR swapchains[] = { vse_app->vk_swapchain };
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signal_semaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &image_index,
        .pResults = NULL,
    };

    vkQueuePresentKHR(vse_app->vk_present_queue, &present_info);    
}

void vse_app_run(VseAppConfig vse_app_config) {
    VseApp *app = vse_app_create(vse_app_config);

    while(!glfwWindowShouldClose(app->window)) {
        glfwPollEvents(); 
        draw_frame(app);

        if(glfwGetKey(app->window, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(app->window, GLFW_TRUE);
    }

    vkDeviceWaitIdle(app->vk_device);
    vse_app_destroy(app);
}

void vse_app_destroy(VseApp* vse_app)
{
    vkDestroySemaphore(vse_app->vk_device, vse_app->semaphore_image_available, NULL);
    vkDestroySemaphore(vse_app->vk_device, vse_app->semaphore_render_finished, NULL);
    vkDestroyFence(vse_app->vk_device, vse_app->fence_inflight, NULL);

    vkDestroyCommandPool(vse_app->vk_device, vse_app->command_pool, NULL);
    for(uint32_t i = 0; i < vse_app->swapchain_image_count; i++) {
        vkDestroyFramebuffer(vse_app->vk_device, vse_app->frame_buffers[i], NULL);
    }
    free(vse_app->frame_buffers);

    vkDestroyPipeline(vse_app->vk_device, vse_app->pipeline, NULL);
    vkDestroyPipelineLayout(vse_app->vk_device, vse_app->pipeline_layout, NULL);
    vkDestroyRenderPass(vse_app->vk_device, vse_app->render_pass, NULL);

    for(uint32_t i = 0; i < vse_app->swapchain_image_count; i++)
        vkDestroyImageView(vse_app->vk_device, vse_app->swapchain_image_views[i], NULL);
    free(vse_app->swapchain_image_views);
    
    free(vse_app->swapchain_images);
    vkDestroySwapchainKHR(vse_app->vk_device, vse_app->vk_swapchain, NULL);
    vkDestroyDevice(vse_app->vk_device, NULL);
    vkDestroySurfaceKHR(vse_app->vk_instance, vse_app->vk_surface, NULL);
    vkDestroyInstance(vse_app->vk_instance, NULL);
    glfwDestroyWindow(vse_app->window);
    glfwTerminate();
    free(vse_app);
    vse_info("Application closed");
}