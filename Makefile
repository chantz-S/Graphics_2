all: main.c intersection.c parser.c
	gcc main.c intersection.c parser.c -o raycast

clean:
	rm -rf main intersection parser *~
