#include "vse.h"
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

void vse_syncobj_create(VseApp *vse_app) {

    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    VkResult create_semaphore_image_result = vkCreateSemaphore(vse_app->vk_device, &semaphore_info, NULL, &vse_app->semaphore_image_available);
    VkResult create_semaphore_render_result = vkCreateSemaphore(vse_app->vk_device, &semaphore_info, NULL, &vse_app->semaphore_render_finished);

    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    VkResult create_fence_result = vkCreateFence(vse_app->vk_device, &fence_info, NULL, &vse_app->fence_inflight);

    if(create_semaphore_image_result != VK_SUCCESS 
    || create_semaphore_render_result != VK_SUCCESS 
    || create_fence_result != VK_SUCCESS) {
        vse_err("Failed to create semaphores.");
        exit(EXIT_FAILURE);
    }

    vse_info("Created semaphores.");
}