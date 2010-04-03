AC_INIT(gutachter,0.0.1,http://github.com/herzi/gutachter/issues)
AM_INIT_AUTOMAKE([no-define dist-bzip2])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PROG_CC
AC_PROG_LIBTOOL
IT_PROG_INTLTOOL([0.35.0])

PKG_CHECK_MODULES([GUTACHTER],[gtk+-2.0])

GETTEXT_PACKAGE="gutachter"
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [The gettext translation domain])
AM_GLIB_GNU_GETTEXT
AC_CONFIG_FILES([po/Makefile.in])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
