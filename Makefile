# C Flags
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -std=c17
CFLAGS += -lm

CFLAGS += -D$(DEMO_NAME)



INCLUDE_FLAGS += -I/opt/homebrew/include
INCLUDE_FLAGS += -L/opt/homebrew/lib

# SDL flags
SDLFLAGS += -lSDL2

# Emscripten flags
EMCCFLAGS += -sUSE_SDL=2
EMCCFLAGS += -sALLOW_MEMORY_GROWTH
EMCCFLAGS += --preload-file ./assets
EMCCFLAGS += --shell-file template.html

EXAMPLES += geometry-example
EXAMPLES += shadow-map-example
EXAMPLES += physics2d-example

build:
	gcc $(CFLAGS) $(INCLUDE_FLAGS) $(SDLFLAGS) ./src/**/*.c ./src/*.c -o $(DEMO_NAME)

build-emscripten:
	emcc $(CFLAGS) $(INCLUDE_FLAGS) $(EMCCFLAGS) ./src/**/*.c ./src/*.c -o docs/examples/$(DEMO_NAME)/index.html

run:
	./$(DEMO_NAME)

clean:
	rm renderer
