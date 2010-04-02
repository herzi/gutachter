# vim:set ft=automake:

icons16dir=$(datadir)/icons/hicolor/16x16/apps
dist_icons16_DATA=pixmaps/16x16/gutachter.png pixmaps/16x16/gutachter.svg
icons22dir=$(datadir)/icons/hicolor/22x22/apps
dist_icons22_DATA=pixmaps/16x16/gutachter.png pixmaps/16x16/gutachter.svg
icons32dir=$(datadir)/icons/hicolor/32x32/apps
dist_icons32_DATA=pixmaps/16x16/gutachter.png pixmaps/16x16/gutachter.svg
icons48dir=$(datadir)/icons/hicolor/48x48/apps
dist_icons48_DATA=pixmaps/16x16/gutachter.png pixmaps/16x16/gutachter.svg

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache

update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi
