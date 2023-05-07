#include "vse.h"
#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

void vse_syncobj_create(VseApp *vse_app) {

    VkSemaphore *semaphores_image_available = malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    VkSemaphore *semaphores_render_finished = malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    VkFence *fences_inflight = malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for(uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult create_semaphore_image_result = vkCreateSemaphore(vse_app->vk_device, &semaphore_info, NULL, &semaphores_image_available[i]);
        VkResult create_semaphore_render_result = vkCreateSemaphore(vse_app->vk_device, &semaphore_info, NULL, &semaphores_render_finished[i]);
        VkResult create_fence_result = vkCreateFence(vse_app->vk_device, &fence_info, NULL, &fences_inflight[i]);

        if(create_semaphore_image_result != VK_SUCCESS 
        || create_semaphore_render_result != VK_SUCCESS 
        || create_fence_result != VK_SUCCESS) {
            vse_err("Failed to create semaphores.");
            exit(EXIT_FAILURE);
        }
    }

    vse_app->semaphores_image_available = semaphores_image_available;
    vse_app->semaphores_render_finished = semaphores_render_finished;
    vse_app->fences_inflight = fences_inflight;

    vse_info("Created semaphores.");
}