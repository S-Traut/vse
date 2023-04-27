#include "vse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

VkBool32 vse_validation_layer_check_support() {

    uint32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties *available_layers = malloc(sizeof(VkLayerProperties) * layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

    for(uint32_t i = 0; i < VALIDATION_LAYERS_COUNT; i++) {
        const char* validation_layer = VALIDATION_LAYERS[i];
        VkBool32 layer_found = VK_FALSE;

        for(uint32_t j = 0; j < layer_count; j++) {
            VkLayerProperties layer_properties = available_layers[j];
            if(strcmp(validation_layer, layer_properties.layerName) == 0) {
                layer_found = VK_TRUE;
                break;
            }
        }

        if(layer_found == VK_FALSE) {
            printf("%s not found.\n", validation_layer);
            return VK_FALSE;
        }
    }

    free(available_layers);
    return VK_TRUE;
}