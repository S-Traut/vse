#include "vse.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

// TODO
// Find a way to generate the details in a clean way without having to malloc weird stuff
void vse_swapchain_query_support(VseSwapchainSupportDetails *swapchain_support_details, VkPhysicalDevice physical_device, VkSurfaceKHR surface) {

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device, 
        surface, 
        &swapchain_support_details->capabilities
    );

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, NULL);
    if(format_count != 0) {
        VkSurfaceFormatKHR formats[format_count];
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats);
        memcpy(swapchain_support_details->formats, formats, sizeof(VkSurfaceFormatKHR) * format_count);
    }
    swapchain_support_details->format_count = format_count;

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &present_mode_count, NULL);
    if(present_mode_count != 0) {
        VkPresentModeKHR present_modes[present_mode_count];
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes);
        memcpy(swapchain_support_details->present_modes, present_modes, sizeof(VkPresentModeKHR) * present_mode_count);
    }
    swapchain_support_details->present_mode_count = present_mode_count;
}