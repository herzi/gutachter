bin_PROGRAMS=
lib_LTLIBRARIES=
noinst_PROGRAMS=
BUILT_SOURCES=
CLEANFILES=
EXTRA_DIST=
TESTS=
SUBDIRS=. po

AM_CPPFLAGS=-Wall -Wextra @GUTACHTER_CFLAGS@

include data/Makefile.inc
include pixmaps/Makefile.inc

bin_PROGRAMS+=gutachter
gutachter_LDADD=libgutachter.la
gutachter_SOURCES=main.c

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=gutachter-$(PACKAGE_VERSION).pc
CLEANFILES+=$(pkgconfig_DATA)
gutachter-$(PACKAGE_VERSION).pc: gutachter.pc
	$(AM_V_GEN) cp $< $@
installcheck-local:
	cd /tmp; echo "#include <gutachter.h>" | cpp -o /dev/null $(shell PKG_CONFIG_PATH=$(pkgconfigdir) pkg-config --cflags gutachter-$(PACKAGE_VERSION))

gutachterdir=$(includedir)/gutachter-$(PACKAGE_VERSION)
gutachter_HEADERS=\
	gutachter.h \
	gutachter-bar.h \
	gutachter-hierarchy.h \
	gutachter-runner.h \
	gutachter-suite.h \
	gutachter-tree-list.h \
	gutachter-types.h \
	gutachter-widget.h \
	gutachter-window.h \
	gutachter-xvfb.h \
	$(NULL)

lib_LTLIBRARIES+=libgutachter.la
libgutachter_la_LDFLAGS=-release $(PACKAGE_VERSION)
libgutachter_la_LIBADD=$(GUTACHTER_LIBS)
libgutachter_la_SOURCES=\
	gutachter-bar.c \
	gutachter-hierarchy.c \
	gutachter-runner.c \
	gutachter-suite.c \
	gutachter-tree-list.c \
	gutachter-widget.c \
	gutachter-window.c \
	gutachter-xvfb.c \
	tango.c \
	tango.h \
	$(NULL)
nodist_libgutachter_la_SOURCES=\
	gutachter-paths.h \
	gutachter-types.c \
	gutachter-types.h \
	$(NULL)
BUILT_SOURCES+=$(nodist_libgutachter_la_SOURCES)
CLEANFILES+=$(nodist_libgutachter_la_SOURCES)

noinst_PROGRAMS+=test-dummy
test_dummy_LDADD=libgutachter.la
test_dummy_SOURCES=test-dummy.c

noinst_PROGRAMS+=test-gutachter
TESTS+=test-gutachter
test_gutachter_LDADD=libgutachter.la
test_gutachter_SOURCES=test-main.c

gutachter-paths.h: Makefile
	$(AM_V_GEN) echo "#define LOCALEDIR \"$(localedir)\"" > $@
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

