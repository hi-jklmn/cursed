all: build

run: build
	./cursed

build: main.c
	cc main.c -lncurses -o cursed
