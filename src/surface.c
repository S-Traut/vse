#include "vse.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xcb.h>
#include <vulkan/vulkan_xlib.h>

void vse_surface_set(VseApp *vse_app) {

    VkResult create_surface_result = glfwCreateWindowSurface(vse_app->vk_instance, vse_app->window, NULL, &vse_app->vk_surface);
    if(create_surface_result != VK_SUCCESS) {
        vse_err("Failed to create Vulkan surface");
        exit(EXIT_FAILURE);
    }

    vse_info("Created window surface.");
}
