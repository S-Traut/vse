#ifndef VSE_H
#define VSE_H

#include "vse_file.h"
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

static const uint32_t DEVICE_EXTENSIONS_COUNT = 1;
static const char* DEVICE_EXTENSIONS[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static const int MAX_FRAMES_IN_FLIGHT = 2;

#define vse_optional(type) struct { VkBool32 present; type value; }

///
/// Optional uint32_t type that can be used to be either present or not.
///
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
    VkQueue vk_present_queue;
    VkSurfaceKHR vk_surface;
    VkSwapchainKHR vk_swapchain;
    uint32_t swapchain_image_count;
    VkImage *swapchain_images;
    VkFormat swapchain_image_format;
    VkExtent2D swapchain_extent;
    VkImageView *swapchain_image_views;
    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;
    VkFramebuffer *frame_buffers;
    VkCommandPool command_pool;
    uint32_t current_frame;
    uint32_t command_buffer_count;
    VkCommandBuffer *command_buffers;
    VkSemaphore *semaphores_image_available;
    VkSemaphore *semaphores_render_finished;
    VkFence *fences_inflight;
    VkBool32 framebuffer_resized;
} VseApp;

typedef struct {
    uint32_o graphics_family;
    uint32_o present_family;
} VseQueueFamilyIndices;

typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32_t format_count;
    VkSurfaceFormatKHR *formats;
    uint32_t present_mode_count;
    VkPresentModeKHR *present_modes;
} VseSwapchainSupportDetails;

void vse_window_create(VseAppConfig vse_app_config, VseApp *vse_app);

void vse_app_run(VseAppConfig vse_app_config);
void vse_app_destroy(VseApp *vse_app); 

VkInstance vse_instance_create(VseAppConfig config);

void vse_device_pick(VseApp *vse_app);
void vse_device_create(VseApp *vse_app);
VkBool32 vse_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
VkBool32 vse_device_check_extension_support(VkPhysicalDevice physical_device);

void vse_swapchain_query_support(VseSwapchainSupportDetails *swapchain_support_details, VkPhysicalDevice physical_device, VkSurfaceKHR surface);
void vse_swapchain_support_details_destroy(VseSwapchainSupportDetails *swapchain_support_details);
VkSurfaceFormatKHR vse_swapchain_pick_format(VkSurfaceFormatKHR *available_formats, uint32_t available_formats_count);
VkPresentModeKHR vse_swapchain_pick_present_mode(VkPresentModeKHR *available_present_modes, uint32_t available_present_modes_count);
VkExtent2D vse_swapchain_pick_extent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR capabilities);
void vse_swapchain_create(VseApp *app);
void vse_swapchain_create_image_views(VseApp *app);
void vse_swapchain_recreate(VseApp *vse_app);
void vse_swapchain_destroy(VseApp *vse_app);

void vse_pipeline_create(VseApp *vse_app);

void vse_renderpass_create(VseApp *vse_app);

void vse_framebuffer_create(VseApp *vse_app);
void vse_framebuffer_resize_callback(GLFWwindow *window, int width, int height);

void vse_command_pool_create(VseApp *vse_app);
void vse_command_buffer_create(VseApp *vse_app);
void vse_command_buffer_record(VseApp vse_app, VkCommandBuffer command_buffer, uint32_t image_index);

void vse_syncobj_create(VseApp *vse_app);

VkShaderModule vse_shadermod_create(VkDevice device, VseFile file);

VkBool32 vse_validation_layer_check_support();

VseQueueFamilyIndices vse_queue_family_find(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
VkBool32 vse_queue_family_iscomplete(VseQueueFamilyIndices queue_family);

void vse_surface_set(VseApp *vse_app);

void vse_logger_extensions();
void vse_logger_physical_devices(VkInstance instance);
void vse_info(const char* message);
void vse_warn(const char* message);
void vse_err_exit(const char* format, ...);


#endif