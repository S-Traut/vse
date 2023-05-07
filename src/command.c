#include "vse.h"
#include <X11/X.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

void vse_command_pool_create(VseApp *vse_app) {

    VseQueueFamilyIndices queue_family_indices = vse_queue_family_find(vse_app->vk_physical_device, vse_app->vk_surface);

    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queue_family_indices.graphics_family.value,
    };

    VkResult create_commandpool_result = vkCreateCommandPool(vse_app->vk_device, &pool_info, NULL, &vse_app->command_pool);
    if(create_commandpool_result != VK_SUCCESS) {
        vse_err_exit("Failed to create command pool.");
    }

    vse_info("Created command pool.");
}

void vse_command_buffer_create(VseApp *vse_app) {

    vse_app->command_buffer_count = MAX_FRAMES_IN_FLIGHT;
    VkCommandBuffer *command_buffers = malloc(sizeof(VkCommandBuffer) * vse_app->command_buffer_count);

    VkCommandBufferAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vse_app->command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = vse_app->command_buffer_count,
    };

    VkResult allocate_command_buffers_result = vkAllocateCommandBuffers(vse_app->vk_device, &allocate_info, command_buffers);
    if(allocate_command_buffers_result != VK_SUCCESS) {
        vse_err_exit("Failed to allocate command buffers.");
    }

    vse_app->command_buffers = command_buffers;
    vse_info("Allocated command buffers.");
}

void vse_command_buffer_record(VseApp vse_app, VkCommandBuffer command_buffer, uint32_t image_index) {

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    VkResult begin_command_buffer_result = vkBeginCommandBuffer(command_buffer, &begin_info);    
    if(begin_command_buffer_result != VK_SUCCESS) {
        vse_err_exit("Failed to begin recording command buffer.");
    }

    VkRenderPassBeginInfo render_pass_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = vse_app.render_pass,
        .framebuffer = vse_app.frame_buffers[image_index],
        .renderArea.offset = {0, 0},
        .renderArea.extent = vse_app.swapchain_extent, 
    };
    
    VkClearValue clear_color = {{{ 0.01f, 0.01f, 0.01f, 1.0f }}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vse_app.pipeline);

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) vse_app.swapchain_extent.width,
        .height = (float) vse_app.swapchain_extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = vse_app.swapchain_extent,
    };
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    vkCmdDraw(command_buffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(command_buffer);
    VkResult end_command_buffer_result = vkEndCommandBuffer(command_buffer);
    if(end_command_buffer_result != VK_SUCCESS) {
        vse_err_exit("Failed to end command buffer.");
    }
}