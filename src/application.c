#include "vse.h"
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

VseApp *vse_app_create(VseAppConfig vse_app_config)
{
    vse_info("STARTING VSE APPLICATION");

    VseApp vse_app;
    vse_app.window = vse_window_create(vse_app_config);
    vse_app.vk_instance = vse_instance_create(vse_app_config);
    vse_surface_set(&vse_app);

    vse_logger_physical_devices(vse_app.vk_instance);
    vse_app.vk_physical_device = vse_device_pick(vse_app.vk_instance, vse_app.vk_surface);
    vse_app.vk_device = vse_device_create(&vse_app);

    VseApp *p_vse_app = malloc(sizeof(VseApp));
    memcpy(p_vse_app, &vse_app, sizeof(VseApp));
    return p_vse_app;
}

void vse_app_run(VseAppConfig vse_app_config) {
    VseApp *app = vse_app_create(vse_app_config);

    while(!glfwWindowShouldClose(app->window)) {
        glfwPollEvents(); 

        if(glfwGetKey(app->window, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(app->window, GLFW_TRUE);
    }

    vse_app_destroy(app);
}

void vse_app_destroy(VseApp* vse_app)
{
    vkDestroyDevice(vse_app->vk_device, NULL);
    vkDestroySurfaceKHR(vse_app->vk_instance, vse_app->vk_surface, NULL);
    vkDestroyInstance(vse_app->vk_instance, NULL);
    glfwDestroyWindow(vse_app->window);
    glfwTerminate();
    free(vse_app);
    vse_info("Application closed");
}