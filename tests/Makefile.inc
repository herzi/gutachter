# vim:set ft=automake:

noinst_PROGRAMS+=test-fail
test_fail_LDADD=$(GUTACHTER_LIBS)
test_fail_SOURCES=tests/test-fail.c

noinst_PROGRAMS+=test-gutachter
TESTS+=test-gutachter
test_gutachter_LDADD=libgutachter.la
test_gutachter_SOURCES=\
	tests/test-hierarchy.c \
	tests/test-lookup.c \
	tests/test-main.h \
	tests/test-main.c \
	tests/test-main.h \
	tests/test-suite.c \
	tests/test-widget.c \
	tests/test-xvfb.c \
	$(NULL)

noinst_PROGRAMS+=test-message
test_message_LDADD=$(GUTACHTER_LIBS)
test_message_SOURCES=tests/test-message.c

noinst_PROGRAMS+=test-pass
test_pass_LDADD=$(GUTACHTER_LIBS)
test_pass_SOURCES=tests/test-pass.c
