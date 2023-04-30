FLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
CC := gcc
SRC := src
OBJ := build/obj
SHD := build/shaders
SOURCES = application.o instance.o window.o logger.o device.o validation.o queue_family.o surface.o swapchain.o vse_math.o pipeline.o vse_file.o shader_module.o main.o
OBJECTS = $(addprefix build/obj/, $(SOURCES))

debug: FLAGS += -g
debug: build

%.o: $(SRC)/%.c
	@mkdir -p build
	@mkdir -p $(OBJ)
	@mkdir -p $(SHD)
	$(CC) $(FLAGS) -I$(SRC) -c $< -o $(OBJ)/$@

run: build
	./build/app

build: $(SOURCES) buildshaders
	gcc $(FLAGS) -Isrc/ $(OBJECTS) -o build/app

buildshaders:
	glslc shaders/shader.vert -o $(SHD)/vert.spv
	glslc shaders/shader.frag -o $(SHD)/frag.spv