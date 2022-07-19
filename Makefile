all: forth

forth: forth.c
	gcc forth.c -o forth -std=c89 -Wall -ggdb

run: forth
	./forth

clean:
	rm -f forth
