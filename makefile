FLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
CC := gcc
SRC := src
OBJ := build/obj
SOURCES = application.o instance.o window.o logger.o device.o validation.o queue_family.o surface.o swapchain.o main.o
OBJECTS = $(addprefix build/obj/, $(SOURCES))

debug: FLAGS += -g
debug: build

%.o: $(SRC)/%.c
	$(CC) $(FLAGS) -I$(SRC) -c $< -o $(OBJ)/$@

run: build
	./build/app

build: $(SOURCES)
	gcc $(FLAGS) -Isrc/ $(OBJECTS) -o build/app