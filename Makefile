build:
	gcc -g -Wall -std=c99 ./src/*.c -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2 -lm -o native-renderer

build-emscripten:
	emcc -Wall -std=c99 ./src/*.c -I/opt/homebrew/include -L/opt/homebrew/lib -sUSE_SDL=2 -sALLOW_MEMORY_GROWTH -lm -o renderer.html --preload-file ./assets --shell-file o.html

run:
	./native-renderer

clean:
	rm renderer
