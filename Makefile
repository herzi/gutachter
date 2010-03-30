AR=@echo "  AR    " $@; ar qcs $@ $^
COMPILE=@echo "  CC    " $@; gcc -c -o $@ $< -g -O2 -Wall -Wextra -I. $(shell pkg-config --cflags gtk+-2.0) -DGETTEXT_PACKAGE=NULL
LINK=@echo "  CCLD  " $@; gcc -o $@ $^ -g -O2 -Wall -Wextra $(shell pkg-config --libs gtk+-2.0)

all: libgutachter.a gutachter test-dummy

clean:
	rm -rf libgutachter.a gutachter test-dummy *.o

gutachter: main.o libgutachter.a
	$(LINK)

gt-types.h: gt-suite.h Makefile
	@echo "  GEN   " $@; glib-mkenums \
		--fhead "#ifndef GUTACHTER_TYPES_H\n#define GUTACHTER_TYPES_H\n\n#include <glib-object.h>" \
		--ftail "\n#endif /* GUTACHTER_TYPES_H */" \
		--eprod "#define GUTACHTER_TYPE_@ENUMSHORT@ (@enum_name@_get_type ())\nGType @enum_name@_get_type (void);" \
		$< > $@
gt-types.c: gt-suite.h Makefile
	@echo "  GEN   " $@; glib-mkenums \
		--fhead "#include <gt-types.h>" \
		--fprod "#include <@filename@>" \
		--eprod "GType @enum_name@_get_type (void)" \
		--vhead "{static GType  stored = 0; if (g_once_init_enter (&stored)) {static G@Type@Value values[] = {" \
		--vprod "{@VALUENAME@, \"@VALUENAME@\", \"@valuenick@\"}," \
		--vtail "{0, NULL, NULL}}; GType registered = g_@type@_register_static(\"@EnumName@\", values); g_once_init_leave (&stored, registered);} return stored;}" \
		$< > $@
gt-types.o: gt-types.c gt-types.h

libgutachter.a: \
	gt-hierarchy.o \
	gt-runner.o \
	gt-types.o \
	gt-suite.o \
	gt-widget.o \
	gt-window.o \
	gt-xvfb-wrapper.o
	$(AR)

%.o: %.c $(wildcard *.h)
	$(COMPILE)

test-dummy: test-dummy.o
	$(LINK)
