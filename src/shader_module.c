#include "vse.h"
#include "vse_file.h"
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

VkShaderModule vse_shadermod_create(VkDevice device, VseFile file) {
    VkShaderModuleCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = file.size,
        .pCode = (uint32_t*)file.data,
    };

    VkShaderModule shader_module;
    VkResult create_shader_module_result = vkCreateShaderModule(device, &create_info, NULL, &shader_module);
    if(create_shader_module_result != VK_SUCCESS) {
        vse_err_exit("Failed to create shader module.");
    }

    return shader_module;
}