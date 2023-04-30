#include "vse.h"
#include "vse_file.h"
#include <vulkan/vulkan_core.h>

void vse_pipeline_create(VseApp app) {

    VseFile vert_shader_file = vse_file_read("build/shaders/frag.spv");
    VseFile frag_shader_file = vse_file_read("build/shaders/vert.spv");
    VkShaderModule vert_shader_module = vse_shadermod_create(app.vk_device, vert_shader_file);
    VkShaderModule frag_shader_module = vse_shadermod_create(app.vk_device, frag_shader_file);

    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vert_shader_module,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = frag_shader_module,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

    vse_info("Created pipeline");

    vse_file_destroy(vert_shader_file);
    vse_file_destroy(frag_shader_file);
    vkDestroyShaderModule(app.vk_device, vert_shader_module, NULL);
    vkDestroyShaderModule(app.vk_device, frag_shader_module, NULL);
}