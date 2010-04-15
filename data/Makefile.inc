# vim:set ft=automake:

applicationsdir=$(datadir)/applications
applications_DATA=gutachter.desktop
CLEANFILES+=$(applications_DATA)
EXTRA_DIST+=data/$(applications_DATA).in.in

@INTLTOOL_DESKTOP_RULE@

CLEANFILES+=$(applications_DATA).in
gutachter.desktop.in: $(srcdir)/data/gutachter.desktop.in.in
	$(AM_V_GEN) sed 's*@BINDIR@*$(bindir)*' $< > $@

check-local: $(applications_DATA)
	@echo "  TEST  " $<; desktop-file-validate $<

