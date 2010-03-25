AR=@echo "  AR    " $@; ar qcs $@ $^
COMPILE=@echo "  CC    " $@; gcc -c -o $@ $< -g -O2 -Wall -Wextra -I. $(shell pkg-config --cflags gtk+-2.0) -DGETTEXT_PACKAGE=NULL
LINK=@echo "  CCLD  " $@; gcc -o $@ $^ -g -O2 -Wall -Wextra $(shell pkg-config --libs gtk+-2.0)

all: libgtk-tester.a gtk-tester test-dummy

clean:
	rm -rf libgtk-tester.a gtk-tester test-dummy *.o

gtk-tester: main.o libgtk-tester.a
	$(LINK)

libgtk-tester.a: \
	gt-widget.o \
	gt-window.o
	$(AR)

%.o: %.c $(wildcard *.h)
	$(COMPILE)

test-dummy: test-dummy.o
	$(LINK)
