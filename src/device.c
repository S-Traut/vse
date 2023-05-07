#include "vse.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>

void vse_device_pick(VseApp *vse_app) {

    VkPhysicalDevice physical_device = VK_NULL_HANDLE;

    uint32_t devices_count = 0;
    vkEnumeratePhysicalDevices(vse_app->vk_instance, &devices_count, NULL);

    if(devices_count == 0) {
        vse_err_exit("No device supporting Vulkan found.");
    }

    VkPhysicalDevice *physical_devices = malloc(sizeof(VkPhysicalDevice) *devices_count);
    vkEnumeratePhysicalDevices(vse_app->vk_instance, &devices_count, physical_devices);

    for(uint32_t i = 0; i < devices_count; i++) {
        if(vse_device_suitable(physical_devices[i], vse_app->vk_surface) == VK_TRUE) {
            physical_device = physical_devices[i];
            break;
        }

        if(physical_device == VK_NULL_HANDLE) {
            vse_err_exit("No suitable device found.");
            free(physical_devices);
        }
    }

    free(physical_devices);
    vse_info("Picked physical device");
    vse_app->vk_physical_device = physical_device;
}

VkBool32 vse_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {

    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &device_features);

    VkBool32 extension_support = vse_device_check_extension_support(physical_device);    
    VseQueueFamilyIndices indices = vse_queue_family_find(physical_device, surface);

    VseSwapchainSupportDetails swapchain_support_details;
    vse_swapchain_query_support(&swapchain_support_details, physical_device, surface);
    VkBool32 swapchain_adequate = VK_FALSE;
    if(extension_support) {
        swapchain_adequate = swapchain_support_details.format_count > 0 && 
        swapchain_support_details.present_mode_count > 0;
    }

    vse_swapchain_support_details_destroy(&swapchain_support_details);
    return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
        && device_features.geometryShader 
        && vse_queue_family_iscomplete(indices)
        && extension_support;
}

void vse_device_create(VseApp *vse_app) {

    VkDevice device;

    float queue_priority = 1.0f;
    VseQueueFamilyIndices indices = vse_queue_family_find(vse_app->vk_physical_device, vse_app->vk_surface);

    // Could be dynamically improved,
    // Right now it's just a basic hard codded solution.
    VkDeviceQueueCreateInfo queue_create_infos[2];
    uint32_t unique_queues[2] = { indices.graphics_family.value, indices.present_family.value };

    for(uint32_t i = 0; i < 2; i++) {
        VkDeviceQueueCreateInfo queue_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = unique_queues[i],
            .queueCount = 1,
            .pQueuePriorities = &queue_priority,
        };
        queue_create_infos[i] = queue_create_info;
    }

    VkPhysicalDeviceFeatures device_features = {}; 
    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = queue_create_infos,
        .queueCreateInfoCount = 2,
        .pEnabledFeatures = &device_features,
        .ppEnabledExtensionNames = DEVICE_EXTENSIONS,
        .enabledExtensionCount = DEVICE_EXTENSIONS_COUNT,
        .enabledLayerCount = VALIDATION_LAYERS_COUNT,
        .ppEnabledLayerNames = VALIDATION_LAYERS,
    };

    VkResult create_device_result = vkCreateDevice(vse_app->vk_physical_device, &create_info, NULL, &device);
    if(create_device_result != VK_SUCCESS) {
        vse_err_exit("Failed to create logical device.");
    }

    vkGetDeviceQueue(device, indices.graphics_family.value, 0, &vse_app->vk_graphics_queue);
    vkGetDeviceQueue(device, indices.present_family.value, 0, &vse_app->vk_present_queue);

    vse_info("Created logical device.");

    vse_app->vk_device = device;
}

VkBool32 vse_device_check_extension_support(VkPhysicalDevice physical_device) {

    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &extension_count, NULL);
    VkExtensionProperties *available_extensions = malloc(sizeof(VkExtensionProperties) * extension_count);
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &extension_count, available_extensions);

    for(uint32_t i = 0; i < DEVICE_EXTENSIONS_COUNT; i++) {
        VkBool32 found = VK_FALSE;

        for(uint32_t j = 0; j < extension_count; j++) {
            if(strcmp(DEVICE_EXTENSIONS[i], available_extensions[j].extensionName) == 0) {
                found = VK_TRUE;
                break;
            }
        }
        
        if(found == VK_FALSE) {
            free(available_extensions);
            return VK_FALSE;
        }
    }

    free(available_extensions);
    return VK_TRUE;
}