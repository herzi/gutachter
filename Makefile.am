bin_PROGRAMS=
lib_LIBRARIES=
noinst_PROGRAMS=
BUILT_SOURCES=
CLEANFILES=
TESTS=

AM_CPPFLAGS=-Wall -Wextra -I. @GUTACHTER_CFLAGS@ -DGETTEXT_PACKAGE=NULL

include pixmaps/Makefile.inc

bin_PROGRAMS+=gutachter
gutachter_LDADD=libgutachter.a $(GUTACHTER_LIBS)
gutachter_SOURCES=main.c

lib_LIBRARIES+=libgutachter.a
libgutachter_a_SOURCES=\
	gutachter.h \
	gutachter-bar.c \
	gutachter-bar.h \
	gutachter-hierarchy.c \
	gutachter-hierarchy.h \
	gutachter-runner.c \
	gutachter-runner.h \
	gutachter-suite.c \
	gutachter-suite.h \
	gutachter-tree-list.c \
	gutachter-tree-list.h \
	gutachter-widget.c \
	gutachter-widget.h \
	gutachter-window.c \
	gutachter-window.h \
	gutachter-xvfb.c \
	gutachter-xvfb.h \
	tango.c \
	tango.h \
	$(NULL)
nodist_libgutachter_a_SOURCES=\
	gutachter-types.c \
	gutachter-types.h \
	$(NULL)
BUILT_SOURCES+=$(nodist_libgutachter_a_SOURCES)
CLEANFILES+=$(nodist_libgutachter_a_SOURCES)

noinst_PROGRAMS+=test-dummy
test_dummy_LDADD=libgutachter.a $(GUTACHTER_LIBS)
test_dummy_SOURCES=test-dummy.c

noinst_PROGRAMS+=test-gutachter
TESTS+=test-gutachter
test_gutachter_LDADD=libgutachter.a $(GUTACHTER_LIBS)
test_gutachter_SOURCES=test-main.c

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

