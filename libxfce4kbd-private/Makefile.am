# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:

if ENABLE_KEYBOARD_LIBRARY

INCLUDES = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	-DDATADIR=\"$(datadir)\" \
	-DG_LOG_DOMAIN=\"libxfce4kbd-private\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	-DPREFIX=\"$(prefix)\" \
	$(PLATFORM_CPPFLAGS)

lib_LTLIBRARIES = libxfce4kbd-private-2.la

libxfce4kbd_headers = \
	xfce-shortcuts-provider.h \
	xfce-shortcuts-grabber.h \
	xfce-shortcut-dialog.h \
	xfce-shortcuts.h

libxfce4kbd_built_sources = \
	xfce-shortcuts-marshal.c \
	xfce-shortcuts-marshal.h

libxfce4kbd_private_includedir = \
	$(includedir)/xfce4/libxfce4kbd-private-$(LIBXFCE4KBD_PRIVATE_VERSION_API)/libxfce4kbd-private

libxfce4kbd_private_include_HEADERS = \
	$(libxfce4kbd_headers)

libxfce4kbd_private_2_la_SOURCES = \
	$(libxfce4kbd_headers) \
	$(libxfce4kbd_built_sources) \
	xfce-shortcuts-provider.c \
	xfce-shortcuts-grabber.c \
	xfce-shortcut-dialog.c \
	xfce-shortcuts.c

libxfce4kbd_private_2_la_CFLAGS = \
	$(PLATFORM_CFLAGS) \
	$(GLIB_CFLAGS)  \
	$(GTK_CFLAGS) \
	$(LIBX11_CFLAGS) \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(XFCONF_CFLAGS)

libxfce4kbd_private_2_la_LDFLAGS = \
	-export-dynamic \
	-version-info $(LIBXFCE4KBD_PRIVATE_VERINFO) \
	-export-symbols-regex "^[^_].*" \
	-no-undefined \
	$(LIBX11_LDFLAGS) \
	$(PLATFORM_LDFLAGS)

libxfce4kbd_private_2_la_LIBADD = \
	$(top_builddir)/libxfce4ui/libxfce4ui-$(LIBXFCE4UI_VERSION_API).la \
	$(GLIB_LIBS) \
	$(GTK_LIBS) \
	$(LIBX11_LIBS) \
	$(LIBXFCE4UTIL_LIBS) \
	$(XFCONF_LIBS)

settingsdir = $(sysconfdir)/xdg/xfce4/xfconf/xfce-perchannel-xml
settings_DATA = xfce4-keyboard-shortcuts.xml

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libxfce4kbd-private-2.pc

if MAINTAINER_MODE
xfce-shortcuts-marshal.h: xfce-shortcuts-marshal.list Makefile
	$(AM_V_GEN) glib-genmarshal --prefix=_xfce_shortcuts_marshal --internal --header $< > $@

xfce-shortcuts-marshal.c: xfce-shortcuts-marshal.list Makefile xfce-shortcuts-marshal.h
	$(AM_V_GEN) echo "#include \"xfce-shortcuts-marshal.h\"" > $@ \
	&& glib-genmarshal --prefix=_xfce_shortcuts_marshal --body $< >> $@

DISTCLEANFILES = \
	$(libxfce4kbd_built_sources)

BUILT_SOURCES = \
	$(libxfce4kbd_built_sources)
endif

EXTRA_DIST = \
	$(settings_DATA) \
	xfce-shortcuts-marshal.list

# required for gtk-doc
dist-hook: all

endif # ENABLE_KEYBOARD_LIBRARY
