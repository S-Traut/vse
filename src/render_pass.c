#include "vse.h"
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

void vse_renderpass_create(VseApp *vse_app) {
    
    VkAttachmentDescription color_attachment = {
        .format = vse_app->swapchain_image_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentReference color_attachment_reference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription sub_pass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_reference,
    };

    VkRenderPassCreateInfo render_pass_create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &color_attachment,
        .subpassCount = 1,
        .pSubpasses = &sub_pass,
    };

    VkResult create_render_pass_result = vkCreateRenderPass(vse_app->vk_device, &render_pass_create_info, NULL, &vse_app->render_pass);
    if(create_render_pass_result != VK_SUCCESS) {
        vse_err("Failed to create render pass.");
        exit(EXIT_FAILURE);
    }

    vse_info("Created render pass.");
}