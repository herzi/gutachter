# vim:set ft=automake:

noinst_PROGRAMS+=test-gutachter
TESTS+=test-gutachter
test_gutachter_LDADD=libgutachter.la
test_gutachter_SOURCES=\
	tests/test-main.c \
	tests/test-main.h \
	tests/test-suite.c \
	tests/test-xvfb.c \
	$(NULL)

noinst_PROGRAMS+=test-pass
test_pass_SOURCES=tests/test-pass.c
test_pass_LDADD=$(GUTACHTER_LIBS)
