#include "vse.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>

VkPhysicalDevice vse_device_pick(VkInstance instance, VkSurfaceKHR surface) {

    VkPhysicalDevice physical_device = VK_NULL_HANDLE;

    uint32_t devices_count = 0;
    vkEnumeratePhysicalDevices(instance, &devices_count, NULL);

    if(devices_count == 0) {
        vse_err("No device supporting Vulkan found.");
        exit(EXIT_FAILURE);
    }

    VkPhysicalDevice *physical_devices = malloc(sizeof(VkPhysicalDevice) *devices_count);
    vkEnumeratePhysicalDevices(instance, &devices_count, physical_devices);

    for(uint32_t i = 0; i < devices_count; i++) {
        if(vse_device_suitable(physical_devices[i], surface) == VK_TRUE) {
            physical_device = physical_devices[i];
            break;
        }

        if(physical_device == VK_NULL_HANDLE) {
            vse_err("No suitable device found.");
            free(physical_devices);
            exit(EXIT_FAILURE);
        }
    }

    free(physical_devices);
    vse_info("Picked physical device");
    return physical_device;
}

VkBool32 vse_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {

    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &device_features);

    VseQueueFamilyIndices indices = vse_queue_family_find(physical_device, surface);

    return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
        && device_features.geometryShader 
        && vse_queue_family_iscomplete(indices)
        && vse_device_check_extension_support(physical_device);
}

VkDevice vse_device_create(VseApp *vse_app) {

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
        vse_err("Failed to create logical device.");
        exit(EXIT_FAILURE);
    }

    vkGetDeviceQueue(device, indices.graphics_family.value, 0, &vse_app->vk_graphics_queue);
    vkGetDeviceQueue(device, indices.present_family.value, 0, &vse_app->vk_present_queue);

    vse_info("Created logical device.");

    return device;
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