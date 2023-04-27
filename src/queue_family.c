#include "vse.h"
#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

VseQueueFamilyIndices vse_queue_family_find(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {

    VseQueueFamilyIndices indices;
    indices.graphics_family.present = VK_FALSE;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
    VkQueueFamilyProperties *queue_families = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families);

    for(uint32_t i = 0; i < queue_family_count; i++) {

        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);

        if(present_support == VK_TRUE) {
            indices.present_family.value = i;
            indices.present_family.present = VK_TRUE;
        }

        if(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family.value = i;
            indices.graphics_family.present = VK_TRUE;
        }
    }

    free(queue_families);
    return indices;
}

VkBool32 vse_queue_family_iscomplete(VseQueueFamilyIndices queue_family) {
    return queue_family.graphics_family.present && queue_family.present_family.present;
}