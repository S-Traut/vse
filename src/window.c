#include "vse.h"
#include <GLFW/glfw3.h>

void vse_window_create(VseAppConfig vse_app_config, VseApp *vse_app) 
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(vse_app_config.width, vse_app_config.height, vse_app_config.name, NULL, NULL);
    glfwSetWindowUserPointer(window, vse_app);
    glfwSetFramebufferSizeCallback(window, vse_framebuffer_resize_callback);
    vse_app->window = window;
}