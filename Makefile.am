bin_PROGRAMS=
lib_LTLIBRARIES=
noinst_PROGRAMS=
BUILT_SOURCES=
CLEANFILES=
DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc
EXTRA_DIST=
TESTS=
SUBDIRS=. doc po
TESTS_ENVIRONMENT=gtester -q

AM_CPPFLAGS=-Wall -Wextra -Wmissing-prototypes @GUTACHTER_CFLAGS@

include data/Makefile.inc
include pixmaps/Makefile.inc
include tests/Makefile.inc

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
	gutachter-lookup.h \
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
	compat-gtk.h \
	gutachter-bar.c \
	gutachter-hierarchy.c \
	gutachter-lookup.c \
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

