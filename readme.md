# VSE - Vulkan Simple Engine

VSE is a vulkan boilerplate that can be used for personal graphics programming projects. Currently, it runs only on Linux / X renderers, but I may add small windows support in the future.

This project follows the popular [Vulkan tutorial](https://vulkan-tutorial.com) and translates it to **C99**. Its goal is to set up a Vulkan instance with the bare minimum needed.

## Compilation

To compile the source code, use the provided makefile. All functions are declared in `src/vse.h`, and the logic is located in the corresponding C files. Please note that the project structure is currently a work in progress and may change in future updates.

This project has been optimized to be compiled on VSCode using launch options. You can easily debug it by pressing `f5` at any time. Alternatively, you can use `make debug` and run `gdb` with the application if you want to run it on something else.

To build the project, simply run `make build` or `make run` to compile and build it.
