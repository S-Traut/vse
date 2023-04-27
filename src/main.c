#include "vse.h"
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

int main(void) {
  vse_app_run((VseAppConfig) {
    .width = 600,
    .height = 600,
    .name = "Vulkan Simple Engine",
  });
  return EXIT_SUCCESS;
}
