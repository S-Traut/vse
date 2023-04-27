#ifndef VSE_H
#define VSE_H

#include <stddef.h>

#define VK_USE_PLATFORM_XCB_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

#include <stdint.h>
#include <vulkan/vulkan_core.h>
#define VSE_VERSION VK_MAKE_API_VERSION(0,1,0,0)

static const uint32_t VALIDATION_LAYERS_COUNT = 1;
static const char* VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation"
};

#define vse_optional(type) struct { VkBool32 present; type value; }
typedef vse_optional(uint32_t) uint32_o;

typedef struct {
    uint32_t width;
    uint32_t height;
    const char* name;
} VseAppConfig;

typedef struct {
    VseAppConfig vse_app_config;
    GLFWwindow *window;
    VkInstance vk_instance;
    VkPhysicalDevice vk_physical_device;
    VkDevice vk_device;
    VkQueue vk_graphics_queue;
    VkSurfaceKHR vk_surface;
} VseApp;

typedef struct {
    uint32_o graphics_family;
    uint32_o present_family;
} VseQueueFamilyIndices;


GLFWwindow* vse_window_create(VseAppConfig vse_app_config);

void vse_app_run(VseAppConfig vse_app_config);
void vse_app_destroy(VseApp *vse_app); 

VkInstance vse_instance_create(VseAppConfig config);

VkPhysicalDevice vse_device_pick(VkInstance instance, VkSurfaceKHR surface);
VkBool32 vse_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
VkDevice vse_device_create(VkPhysicalDevice physical_device, VkQueue graphics_queue, VkSurfaceKHR surface);

VkBool32 vse_validation_layer_check_support();

VseQueueFamilyIndices vse_queue_family_find(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
VkBool32 vse_queue_family_iscomplete(VseQueueFamilyIndices queue_family);

void vse_surface_set(VseApp *vse_app);


void vse_logger_extensions();
void vse_logger_physical_devices(VkInstance instance);
void vse_info(const char* message);
void vse_err(const char* message);


#endif