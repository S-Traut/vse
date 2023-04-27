#include "vse.h"
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

// TODO
// Find a way to generate the details in a clean way without having to malloc weird stuff
SwapchainSupportDetails vse_swapchain_query_support(VseApp *vse_app) {
    SwapchainSupportDetails swapchain_support_details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        vse_app->vk_physical_device, 
        vse_app->vk_surface, 
        &swapchain_support_details.capabilities
    );

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vse_app->vk_physical_device, vse_app->vk_surface, &format_count, NULL);
    if(format_count != 0) {
        VkSurfaceFormatKHR *p_formats = malloc(sizeof(VkSurfaceFormatKHR) * format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vse_app->vk_physical_device, vse_app->vk_surface, &format_count, p_formats);
        memcpy(swapchain_support_details.formats, p_formats, sizeof(VkSurfaceFormatKHR) * format_count);
    }

    return swapchain_support_details; 
}