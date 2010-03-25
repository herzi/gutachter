COMPILE=@echo "  CCLD  " $@; gcc -o $@ $< -g -O2 -Wall -Wextra $(shell pkg-config --cflags --libs gtk+-2.0)
all: gtk-tester test-dummy

clean:
	rm -rf gtk-tester test-dummy

gtk-tester: main.c
	$(COMPILE)

test-dummy: test-dummy.c
	$(COMPILE)
