#include "vse.h"
#include "vse_math.h"
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>

void vse_swapchain_query_support(VseSwapchainSupportDetails *swapchain_support_details, VkPhysicalDevice physical_device, VkSurfaceKHR surface) {

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device, 
        surface, 
        &swapchain_support_details->capabilities
    );

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, NULL);
    if(format_count != 0) {
        VkSurfaceFormatKHR *formats = malloc(sizeof(VkSurfaceFormatKHR) * format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats);
        swapchain_support_details->formats = formats;
    }
    swapchain_support_details->format_count = format_count;

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &present_mode_count, NULL);
    if(present_mode_count != 0) {
        VkPresentModeKHR *present_modes = malloc(sizeof(VkPresentModeKHR) * present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes);
        swapchain_support_details->present_modes = present_modes;
    }
    swapchain_support_details->present_mode_count = present_mode_count;
}

void vse_swapchain_support_details_destroy(VseSwapchainSupportDetails *swapchain_support_details) {
    free(swapchain_support_details->formats);
    free(swapchain_support_details->present_modes);
}

VkSurfaceFormatKHR vse_swapchain_pick_format(VkSurfaceFormatKHR *available_formats, uint32_t available_formats_count) {
    for(uint32_t i = 0; i < available_formats_count; i++) {
        VkSurfaceFormatKHR available_format = available_formats[i];
        if(available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
            return available_format;
        }
    }

    vse_warn("No suitable format found.");
    return available_formats[0];
}

VkPresentModeKHR vse_swapchain_pick_present_mode(VkPresentModeKHR *available_present_modes, uint32_t available_present_modes_count) {
    for(uint32_t i = 0; i < available_present_modes_count; i++) {
        VkPresentModeKHR available_present_mode = available_present_modes[i];
        if(available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available_present_mode;
        }
    }

    vse_warn("No suitable present mode fount, using VK_PRESENT_MODE_FIFO_KHR.");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D vse_swapchain_pick_extent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR capabilities) {
    
    if(capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actual_extent = {
        (uint32_t) width,
        (uint32_t) height,
    };

    actual_extent.width = vse_math_clamp_uint(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual_extent.height = vse_math_clamp_uint(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actual_extent;
}

void vse_swapchain_create(VseApp *vse_app) {
    VseSwapchainSupportDetails swapchain_support_details;
    vse_swapchain_query_support(&swapchain_support_details, vse_app->vk_physical_device, vse_app->vk_surface);

    VkSurfaceFormatKHR surface_format = vse_swapchain_pick_format(swapchain_support_details.formats, swapchain_support_details.format_count);
    VkPresentModeKHR present_mode = vse_swapchain_pick_present_mode(swapchain_support_details.present_modes, swapchain_support_details.present_mode_count);
    VkExtent2D extent = vse_swapchain_pick_extent(vse_app->window, swapchain_support_details.capabilities);

    uint32_t image_count = swapchain_support_details.capabilities.minImageCount + 1;
    if(swapchain_support_details.capabilities.maxImageCount > 0 && image_count > swapchain_support_details.capabilities.maxImageCount) {
        image_count = swapchain_support_details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = vse_app->vk_surface,
        .minImageCount = image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    };

    VseQueueFamilyIndices indices = vse_queue_family_find(vse_app->vk_physical_device, vse_app->vk_surface);
    uint32_t queue_family_indices[] = { indices.graphics_family.value, indices.present_family.value };

    if(indices.graphics_family.value != indices.present_family.value) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = NULL;
    }

    create_info.preTransform = swapchain_support_details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;
    
    VkSwapchainKHR swapchain;
    VkResult create_swapchain_result = vkCreateSwapchainKHR(vse_app->vk_device, &create_info, NULL, &swapchain);
    if(create_swapchain_result != VK_SUCCESS) {
        vse_err_exit("Failed to create swapchain.");
    }

    vkGetSwapchainImagesKHR(vse_app->vk_device, swapchain, &image_count, NULL);
    vse_app->swapchain_images = malloc(sizeof(VkImage) * image_count);
    vkGetSwapchainImagesKHR(vse_app->vk_device, swapchain, &image_count, vse_app->swapchain_images);

    vse_app->swapchain_image_count = image_count;
    vse_app->swapchain_image_format = surface_format.format;
    vse_app->swapchain_extent = extent;
    vse_app->vk_swapchain = swapchain;
}

void vse_swapchain_create_image_views(VseApp *vse_app) {

    VkImageView *image_views = malloc(sizeof(VkImageView) * vse_app->swapchain_image_count);

    for(uint32_t i = 0; i < vse_app->swapchain_image_count; i++) {
        VkImageViewCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = vse_app->swapchain_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = vse_app->swapchain_image_format,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1,
        };

        VkResult create_image_view_result = vkCreateImageView(vse_app->vk_device, &create_info, NULL, &image_views[i]);
        if(create_image_view_result != VK_SUCCESS) {
            vse_err_exit("Failed to create image view.");
        }
    }

    vse_info("Created image views.");
    vse_app->swapchain_image_views = image_views;
}

void vse_swapchain_recreate(VseApp *vse_app) {

    vkDeviceWaitIdle(vse_app->vk_device);

    vse_swapchain_destroy(vse_app);

    vse_swapchain_create(vse_app);
    vse_swapchain_create_image_views(vse_app);
    vse_framebuffer_create(vse_app);
}

void vse_swapchain_destroy(VseApp *vse_app) {

    for(uint32_t i = 0; i < vse_app->swapchain_image_count; i++) {
        vkDestroyFramebuffer(vse_app->vk_device, vse_app->frame_buffers[i], NULL);
        vkDestroyImageView(vse_app->vk_device, vse_app->swapchain_image_views[i], NULL);
    }

    vkDestroySwapchainKHR(vse_app->vk_device, vse_app->vk_swapchain, NULL);
}