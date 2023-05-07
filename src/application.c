#include "vse.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

VseApp *vse_app_create(VseAppConfig vse_app_config)
{
    vse_info("STARTING VSE APPLICATION");

    VseApp vse_app;

    vse_window_create(vse_app_config, &vse_app);
    vse_app.vk_instance = vse_instance_create(vse_app_config);

    vse_surface_set(&vse_app);
    vse_logger_physical_devices(vse_app.vk_instance);
    vse_device_pick(&vse_app);
    vse_device_create(&vse_app);
    vse_swapchain_create(&vse_app);
    vse_swapchain_create_image_views(&vse_app);
    vse_renderpass_create(&vse_app);
    vse_pipeline_create(&vse_app);
    vse_framebuffer_create(&vse_app);
    vse_command_pool_create(&vse_app);
    vse_command_buffer_create(&vse_app);
    vse_syncobj_create(&vse_app);

    vse_app.framebuffer_resized = VK_FALSE;
    vse_app.current_frame = 0;

    VseApp *p_vse_app = malloc(sizeof(VseApp));
    memcpy(p_vse_app, &vse_app, sizeof(VseApp));
    return p_vse_app;
}

// TO BE CHANGED TO FUNCTION POINTER IN THE APP CONFIG
void draw_frame(VseApp *vse_app) {

    uint32_t current_frame = vse_app->current_frame;

    VkFence fences = vse_app->fences_inflight[current_frame];
    vkWaitForFences(vse_app->vk_device, 1, &fences, VK_TRUE, UINT64_MAX);

    uint32_t image_index;
    VkResult aquire_next_image_result = vkAcquireNextImageKHR(vse_app->vk_device, vse_app->vk_swapchain, UINT64_MAX, vse_app->semaphores_image_available[current_frame], VK_NULL_HANDLE, &image_index);
    if(aquire_next_image_result == VK_ERROR_OUT_OF_DATE_KHR) {
        vse_swapchain_recreate(vse_app);
        return;
    } else if(aquire_next_image_result != VK_SUCCESS && aquire_next_image_result != VK_SUBOPTIMAL_KHR) {
        vse_err_exit("Failed to acquire swapchain image.");
    }
    
    vkResetFences(vse_app->vk_device, 1, &vse_app->fences_inflight[current_frame]);


    vkResetCommandBuffer(vse_app->command_buffers[current_frame], 0);
    vse_command_buffer_record(*vse_app, vse_app->command_buffers[current_frame], image_index);

    VkSemaphore wait_semaphores[] = { vse_app->semaphores_image_available[current_frame] };
    VkSemaphore signal_semaphores[] = { vse_app->semaphores_render_finished[current_frame] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,  
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = wait_semaphores,
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &vse_app->command_buffers[current_frame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signal_semaphores,
    };

    VkResult queue_submit_result = vkQueueSubmit(vse_app->vk_graphics_queue, 1, &submit_info, vse_app->fences_inflight[current_frame]);
    if(queue_submit_result != VK_SUCCESS) {
        vse_err_exit("Failed to submit queue.");
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

    VkResult queue_present_result = vkQueuePresentKHR(vse_app->vk_present_queue, &present_info);    

    if(queue_present_result == VK_ERROR_OUT_OF_DATE_KHR 
    || queue_present_result == VK_SUBOPTIMAL_KHR 
    || vse_app->framebuffer_resized == VK_TRUE) {
        vse_app->framebuffer_resized = VK_FALSE;
        vse_swapchain_recreate(vse_app);
    } else if(queue_present_result != VK_SUCCESS) {
        vse_err_exit("Failed to present swapchain image.");
    }
    
    vse_app->current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
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
    vse_swapchain_destroy(vse_app);
    free(vse_app->frame_buffers);
    free(vse_app->swapchain_image_views);
    free(vse_app->swapchain_images);

    vkDestroyPipeline(vse_app->vk_device, vse_app->pipeline, NULL);
    vkDestroyPipelineLayout(vse_app->vk_device, vse_app->pipeline_layout, NULL);
    vkDestroyRenderPass(vse_app->vk_device, vse_app->render_pass, NULL);

    for(uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(vse_app->vk_device, vse_app->semaphores_image_available[i], NULL);
        vkDestroySemaphore(vse_app->vk_device, vse_app->semaphores_render_finished[i], NULL);
        vkDestroyFence(vse_app->vk_device, vse_app->fences_inflight[i], NULL);
    }

    free(vse_app->semaphores_image_available);
    free(vse_app->semaphores_render_finished);
    free(vse_app->command_buffers);

    vkDestroyCommandPool(vse_app->vk_device, vse_app->command_pool, NULL);
    vkDestroyDevice(vse_app->vk_device, NULL);

    vkDestroySurfaceKHR(vse_app->vk_instance, vse_app->vk_surface, NULL);
    vkDestroyInstance(vse_app->vk_instance, NULL);

    glfwDestroyWindow(vse_app->window);
    glfwTerminate();
    free(vse_app);
    vse_info("Application closed");
}