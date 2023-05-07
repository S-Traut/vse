#include "vse.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

VkInstance vse_instance_create(VseAppConfig vse_app_config) {

	vse_logger_extensions();
	if(!vse_validation_layer_check_support()) {
		exit(EXIT_FAILURE);
	}

    VkInstance vk_instance;    

    VkApplicationInfo application_info = {
    	.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    	.pNext = NULL,
    	.pApplicationName = vse_app_config.name,
    	.applicationVersion = VK_MAKE_VERSION(1,0,0),
    	.pEngineName = "VSE",
    	.engineVersion = VSE_VERSION,
    	.apiVersion = VK_API_VERSION_1_0,
    }; 

    uint32_t required_extension_count = 0;
    const char** required_extensions;
    required_extensions = glfwGetRequiredInstanceExtensions(&required_extension_count);

    VkInstanceCreateInfo create_info = {
    	.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    	.pApplicationInfo = &application_info,
    	.enabledLayerCount = VALIDATION_LAYERS_COUNT,	
		.ppEnabledLayerNames = VALIDATION_LAYERS,
    	.enabledExtensionCount = required_extension_count,
    	.ppEnabledExtensionNames = required_extensions,
    };  

    VkResult create_instance_result = vkCreateInstance(&create_info, NULL, &vk_instance);
    if(create_instance_result != VK_SUCCESS) {
    	vse_err_exit("An error occured while trying to create vkInstance");
    }

	vse_info("Created Vulkan instance.");

    return vk_instance;
}