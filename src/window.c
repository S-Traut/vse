#include "vse.h"
#include <GLFW/glfw3.h>

GLFWwindow *vse_window_create(VseAppConfig vse_app_config) 
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    return glfwCreateWindow(vse_app_config.width, vse_app_config.height, vse_app_config.name, NULL, NULL);
}