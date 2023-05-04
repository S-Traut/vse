#include "vse.h"
#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

void vse_framebuffer_create(VseApp *vse_app) {

    VkFramebuffer *p_framebuffers = malloc(sizeof(VkFramebuffer) * vse_app->swapchain_image_count);

    for(uint32_t i = 0; i < vse_app->swapchain_image_count; i++) {
        
        VkImageView attachments[] = {
            vse_app->swapchain_image_views[i],
        };

        VkFramebufferCreateInfo framebuffer_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = vse_app->render_pass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = vse_app->swapchain_extent.width,
            .height = vse_app->swapchain_extent.height,
            .layers = 1,
        };

        VkResult create_framebuffer_result = vkCreateFramebuffer(vse_app->vk_device, &framebuffer_info, NULL, &p_framebuffers[i]);
        if(create_framebuffer_result != VK_SUCCESS) {
            vse_err("Failed to create framebuffer.");
            exit(EXIT_FAILURE);
        }
    }

    vse_app->frame_buffers = p_framebuffers;
    vse_info("Created framebuffers.");
}