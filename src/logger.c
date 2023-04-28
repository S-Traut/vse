#include "vse.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

void vse_info(const char* message) {
    printf("[I] %s\n", message);
}

void vse_err(const char* message) {
    printf("[E] %s\n", message);
}

void vse_warn(const char* message) {
    printf("[W] %s\n", message);
}

void vse_logger_extensions() {

	uint32_t extensions_count = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extensions_count, NULL);

	VkExtensionProperties *properties = malloc(sizeof(VkExtensionProperties) * extensions_count);
	vkEnumerateInstanceExtensionProperties(NULL, &extensions_count, properties);

	vse_info("Available extensions:");
	for(uint32_t i = 0; i < extensions_count; i++) {
		printf("%s ", properties[i].extensionName);
	}
	printf("\n");

    free(properties);
}

void vse_logger_physical_devices(VkInstance instance) {

    uint32_t devices_count = 0;
    vkEnumeratePhysicalDevices(instance, &devices_count, NULL);

    VkPhysicalDevice *physical_devices = malloc(sizeof(VkPhysicalDevice) *devices_count);
    vkEnumeratePhysicalDevices(instance, &devices_count, physical_devices);

    vse_info("Available physical devices:");
    for(uint32_t i = 0; i < devices_count; i++) {
        VkPhysicalDeviceProperties physical_device_properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);
        printf("\t%s\n", physical_device_properties.deviceName);
    }

    free(physical_devices);
}