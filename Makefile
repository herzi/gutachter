all: gtk-tester

gtk-tester: main.c
	gcc -o $@ $< -g -O2 -Wall -Wextra $(shell pkg-config --cflags --libs gtk+-2.0)
