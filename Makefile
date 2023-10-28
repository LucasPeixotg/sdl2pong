build: 
	gcc ./src/*.c -Wall -pedantic -Wextra -std=c99 -lSDL2 -lm -o game

run:
	./game

clean:
	rm game
