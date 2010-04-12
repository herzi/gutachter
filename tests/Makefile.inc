# vim:set ft=automake:

noinst_PROGRAMS+=test-pass
test_pass_SOURCES=tests/test-pass.c
test_pass_LDADD=$(GUTACHTER_LIBS)
