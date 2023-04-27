#include "vse.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
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
        && vse_queue_family_iscomplete(indices);
}

VkDevice vse_device_create(VkPhysicalDevice physical_device, VkQueue graphics_queue, VkSurfaceKHR surface) {

    VkDevice device;

    float queue_priority = 1.0f;
    VseQueueFamilyIndices indices = vse_queue_family_find(physical_device, surface);
    VkDeviceQueueCreateInfo queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = indices.graphics_family.value,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };

    VkPhysicalDeviceFeatures device_features = {}; 
    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = &queue_create_info,
        .queueCreateInfoCount = 1,
        .pEnabledFeatures = &device_features,
        .enabledExtensionCount = 0,
        .enabledLayerCount = VALIDATION_LAYERS_COUNT,
        .ppEnabledLayerNames = VALIDATION_LAYERS,
    };

    VkResult create_device_result = vkCreateDevice(physical_device, &create_info, NULL, &device);
    if(create_device_result != VK_SUCCESS) {
        vse_err("Failed to create logical device.");
        exit(EXIT_FAILURE);
    }

    vkGetDeviceQueue(device, indices.graphics_family.value, 0, &graphics_queue);

    vse_info("Created logical device.");

    return device;
}