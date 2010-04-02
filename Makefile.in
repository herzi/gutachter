AR=@echo "  AR    " $@; ar qcs $@ $^
COMPILE=@echo "  CC    " $@; gcc -c -o $@ $< -g -O2 -Wall -Wextra -I. $(shell pkg-config --cflags gtk+-2.0) -DGETTEXT_PACKAGE=NULL
LINK=@echo "  CCLD  " $@; gcc -o $@ $^ -g -O2 -Wall -Wextra $(shell pkg-config --libs gtk+-2.0)

all: libgutachter.a gutachter test-dummy test-gutachter

check: test-gutachter
	./test-gutachter

clean:
	rm -rf libgutachter.a gutachter test-dummy *.o gutachter-types.c gutachter-types.h

gutachter: main.o libgutachter.a
	$(LINK)

gutachter-types.h: gutachter-suite.h Makefile
	@echo "  GEN   " $@; glib-mkenums \
		--fhead "#ifndef GUTACHTER_TYPES_H\n#define GUTACHTER_TYPES_H\n\n#include <glib-object.h>" \
		--ftail "\n#endif /* GUTACHTER_TYPES_H */" \
		--eprod "#define GUTACHTER_TYPE_@ENUMSHORT@ (@enum_name@_get_type ())\nGType @enum_name@_get_type (void);" \
		$< > $@
gutachter-types.c: gutachter-suite.h Makefile
	@echo "  GEN   " $@; glib-mkenums \
		--fhead "#include <gutachter-types.h>" \
		--fprod "#include <@filename@>" \
		--eprod "GType @enum_name@_get_type (void)" \
		--vhead "{static GType  stored = 0; if (g_once_init_enter (&stored)) {static G@Type@Value values[] = {" \
		--vprod "{@VALUENAME@, \"@VALUENAME@\", \"@valuenick@\"}," \
		--vtail "{0, NULL, NULL}}; GType registered = g_@type@_register_static(\"@EnumName@\", values); g_once_init_leave (&stored, registered);} return stored;}" \
		$< > $@
gutachter-types.o: gutachter-types.c gutachter-types.h

libgutachter.a: \
	gutachter-bar.o \
	gutachter-hierarchy.o \
	gutachter-runner.o \
	gutachter-suite.o \
	gutachter-tree-list.o \
	gutachter-types.o \
	gutachter-widget.o \
	gutachter-window.o \
	gutachter-xvfb.o \
	tango.o
	$(AR)

%.o: %.c $(wildcard *.h) gutachter-types.h
	$(COMPILE)

test-dummy: test-dummy.o
	$(LINK)

test-gutachter: test-main.o libgutachter.a
	$(LINK)
