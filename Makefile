build:
	gcc -Wall -std=c99 ./src/*.c -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2 -lm -o renderer

run:
	./renderer

clean:
	rm renderer
