## Process this file with automake to produce Makefile.in

# We require automake 1.6 at least.
AUTOMAKE_OPTIONS = 1.6

# tell GNU make that we don't like anything but "make -j1"
.NOTPARALLEL:

# The name of the module, e.g. 'glib'.
DOC_MODULE=gutachter

# Uncomment for versioned docs and specify the version of the module, e.g. '2'.
#DOC_MODULE_VERSION=2


# The top-level SGML file. You can change this if you want to.
DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.sgml

# The directory containing the source code. Relative to $(srcdir).
# gtk-doc will search all .c & .h files beneath here for inline comments
# documenting the functions and macros.
# e.g. DOC_SOURCE_DIR=../../../gtk
DOC_SOURCE_DIR=..

# Extra options to pass to gtkdoc-scangobj. Not normally needed.
SCANGOBJ_OPTIONS=

# Extra options to supply to gtkdoc-scan.
# e.g. SCAN_OPTIONS=--deprecated-guards="GTK_DISABLE_DEPRECATED"
SCAN_OPTIONS=

# Extra options to supply to gtkdoc-mkdb.
# e.g. MKDB_OPTIONS=--sgml-mode --output-format=xml
MKDB_OPTIONS=--sgml-mode --output-format=xml

# Extra options to supply to gtkdoc-mktmpl
# e.g. MKTMPL_OPTIONS=--only-section-tmpl
MKTMPL_OPTIONS=

# Extra options to supply to gtkdoc-mkhtml
MKHTML_OPTIONS=

# Extra options to supply to gtkdoc-fixref. Not normally needed.
# e.g. FIXXREF_OPTIONS=--extra-dir=../gdk-pixbuf/html --extra-dir=../gdk/html
FIXXREF_OPTIONS=

# Used for dependencies. The docs will be rebuilt if any of these change.
# e.g. HFILE_GLOB=$(top_srcdir)/gtk/*.h
# e.g. CFILE_GLOB=$(top_srcdir)/gtk/*.c
HFILE_GLOB=$(top_srcdir)/*.h
CFILE_GLOB=$(top_srcdir)/*.c

$(DOC_MODULE).types: $(H_FILE_GLOB)

# Header files to ignore when scanning.
# e.g. IGNORE_HFILES=gtkdebug.h gtkintl.h
IGNORE_HFILES=compat-gtk.h tango.h test-main.h

# Images to copy into HTML directory.
# e.g. HTML_IMAGES=$(top_srcdir)/gtk/stock-icons/stock_about_24.png
HTML_IMAGES=

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE).
# e.g. content_files=running.sgml building.sgml changes-2.0.sgml
content_files=

# SGML files where gtk-doc abbrevations (#GtkWidget) are expanded
# These files must be listed here *and* in content_files
# e.g. expand_content_files=running.sgml
expand_content_files=

# CFLAGS and LDFLAGS for compiling gtkdoc-scangobj with your library.
# Only needed if you are using gtkdoc-scangobj to dynamically query widget
# signals and properties.
# e.g. GTKDOC_CFLAGS=-I$(top_srcdir) -I$(top_builddir) $(GTK_DEBUG_FLAGS)
# e.g. GTKDOC_LIBS=$(top_builddir)/gtk/$(gtktargetlib)
GTKDOC_CFLAGS=$(GUTACHTER_CFLAGS)
GTKDOC_LIBS=../libgutachter.la $(GUTACHTER_LIBS)

# This includes the standard gtk-doc make rules, copied by gtkdocize.
include $(top_srcdir)/gtk-doc.make

# Other files to distribute
# e.g. EXTRA_DIST += version.xml.in
EXTRA_DIST +=

# Files not to distribute
# for --rebuild-types in $(SCAN_OPTIONS), e.g. $(DOC_MODULE).types
# for --rebuild-sections in $(SCAN_OPTIONS) e.g. $(DOC_MODULE)-sections.txt
CLEANFILES+=$(DOC_MODULE).types $(DOC_MODULE)-sections.txt

# Comment this out if you want your docs-status tested during 'make check'
#FIXME: enable TESTS = $(GTKDOC_CHECK)

