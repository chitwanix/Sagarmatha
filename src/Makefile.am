NULL = 
BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =
bin_SCRIPTS =
pkglibexec_PROGRAMS =
noinst_LTLIBRARIES =
noinst_PROGRAMS =
service_in_files =

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --warn-all --warn-error --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir) --includedir=$(MUFFIN_TYPELIB_DIR)

typelibdir = $(pkglibdir)
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

servicedir = $(datadir)/dbus-1/services
service_DATA = $(service_in_files:.service.in=.service)

%.service: %.service.in Makefile
	$(AM_V_GEN)									\
		[ -d $(@D) ] || $(MKDIR_P) $(@D) ;					\
		sed -e "s|\@libexecdir\@|$(libexecdir)|" \
		    -e "s|\@pkglibexecdir\@|$(pkglibexecdir)|" $< > $@.tmp && mv $@.tmp $@

CLEANFILES += $(service_DATA)

CLEANFILES += $(gir_DATA) $(typelib_DATA)

bin_SCRIPTS += sagarmatha-extension-tool
EXTRA_DIST += sagarmatha-extension-tool.in
bin_PROGRAMS = sagarmatha-real

if USE_JHBUILD_WRAPPER_SCRIPT
sagarmatha = sagarmatha-jhbuild
bin_SCRIPTS += sagarmatha-jhbuild
else
sagarmatha = sagarmatha-real
endif
EXTRA_DIST += sagarmatha-jhbuild.in

noinst_DATA = sagarmatha
sagarmatha: $(sagarmatha) Makefile
	$(AM_V_GEN) cp $< $@.tmp && mv $@.tmp $@
install-exec-hook:
	mv $(DESTDIR)$(bindir)/$(sagarmatha) $(DESTDIR)$(bindir)/sagarmatha
uninstall-hook:
	rm -f $(DESTDIR)$(bindir)/sagarmatha

generated_script_substitutions = \
	    -e "s|@bindir[@]|$(bindir)|" \
	    -e "s|@datadir[@]|$(datadir)|" \
	    -e "s|@libexecdir[@]|$(libexecdir)|" \
	    -e "s|@libdir[@]|$(libdir)|" \
	    -e "s|@JHBUILD_TYPELIBDIR[@]|$(JHBUILD_TYPELIBDIR)|" \
	    -e "s|@pkgdatadir[@]|$(pkgdatadir)|" \
	    -e "s|@pkglibexecdir[@]|$(pkglibexecdir)|" \
	    -e "s|@PYTHON[@]|$(PYTHON)|" \
	    -e "s|@VERSION[@]|$(VERSION)|" \
	    -e "s|@sysconfdir[@]|$(sysconfdir)|"

sagarmatha-jhbuild: sagarmatha-jhbuild.in sagarmatha-real Makefile
	$(AM_V_GEN) sed $(generated_script_substitutions) $< > $@.tmp && mv $@.tmp $@ && chmod a+x $@
	@# Regenerate libtool wrapper so Alt+F2 restart will work
	@$(LIBTOOL) --mode=execute test -f sagarmatha-real

sagarmatha-extension-tool: sagarmatha-extension-tool.in Makefile
	$(AM_V_GEN) sed $(generated_script_substitutions) $< > $@.tmp && mv $@.tmp $@ && chmod a+x $@

CLEANFILES += sagarmatha $(bin_SCRIPTS)

include Makefile-st.am
include Makefile-tray.am
include Makefile-gvc.am
include Makefile-hotplug-sniffer.am

sagarmatha_cflags =				\
	$(CINNAMON_CFLAGS)			\
	-I$(srcdir)/tray			\
	-DVERSION=\"$(VERSION)\"		\
	-DLOCALEDIR=\"$(datadir)/sagarmatha/locale\" 	\
	-DDATADIR=\"$(datadir)\"		\
	-DCINNAMON_LIBEXECDIR=\"$(libexecdir)\"	\
	-DCINNAMON_DATADIR=\"$(pkgdatadir)\"	\
	-DCINNAMON_PKGLIBDIR=\"$(pkglibdir)\" \
	-DCINNAMON_PKGLIBEXECDIR=\"$(pkglibexecdir)\"	\
	-DJSDIR=\"$(pkgdatadir)/js\"

privlibdir = $(pkglibdir)
privlib_LTLIBRARIES = libsagarmatha.la

sagarmatha_built_sources = \
	sagarmatha-marshal.h \
	sagarmatha-marshal.c \
	sagarmatha-enum-types.h \
	sagarmatha-enum-types.c

BUILT_SOURCES += $(sagarmatha_built_sources)
EXTRA_DIST += sagarmatha-marshal.list

sagarmatha_public_headers_h =		\
	sagarmatha-app.h			\
	sagarmatha-app-system.h		\
	sagarmatha-arrow.h			\
	sagarmatha-doc-system.h		\
	sagarmatha-embedded-window.h		\
	sagarmatha-generic-container.h	\
	sagarmatha-gtk-embed.h		\
	sagarmatha-global.h			\
	sagarmatha-mount-operation.h		\
	sagarmatha-perf-log.h		\
	sagarmatha-screenshot.h	\
	sagarmatha-screen-grabber.h	\
	sagarmatha-slicer.h			\
	sagarmatha-stack.h			\
	sagarmatha-tray-icon.h		\
	sagarmatha-tray-manager.h		\
	sagarmatha-util.h			\
	sagarmatha-window-tracker.h		\
	sagarmatha-wm.h			\
	sagarmatha-xfixes-cursor.h

libsagarmatha_la_SOURCES =		\
	$(sagarmatha_built_sources)		\
	$(sagarmatha_public_headers_h)	\
	sagarmatha-app-private.h		\
	sagarmatha-app-system-private.h	\
	sagarmatha-embedded-window-private.h	\
	sagarmatha-global-private.h		\
	sagarmatha-jsapi-compat-private.h	\
	sagarmatha-window-tracker-private.h	\
	sagarmatha-wm-private.h		\
	sagarmatha-plugin.c		\
	sagarmatha-app.c			\
	sagarmatha-a11y.h			\
	sagarmatha-a11y.c			\
	sagarmatha-app-system.c		\
	sagarmatha-arrow.c			\
	sagarmatha-doc-system.c		\
	sagarmatha-embedded-window.c		\
	sagarmatha-generic-container.c	\
	sagarmatha-gtk-embed.c		\
	sagarmatha-global.c			\
	sagarmatha-mount-operation.c		\
	sagarmatha-perf-log.c		\
	sagarmatha-screenshot.c		\
	sagarmatha-screen-grabber.c	\
	sagarmatha-slicer.c			\
	sagarmatha-stack.c			\
	sagarmatha-tray-icon.c		\
	sagarmatha-tray-manager.c		\
	sagarmatha-util.c			\
	sagarmatha-window-tracker.c		\
	sagarmatha-wm.c			\
	sagarmatha-xfixes-cursor.c

libsagarmatha_la_gir_sources = \
	$(filter-out %-private.h $(sagarmatha_recorder_non_gir_sources), $(sagarmatha_public_headers_h) $(libsagarmatha_la_SOURCES))

sagarmatha_real_SOURCES =		\
	main.c
sagarmatha_real_CPPFLAGS = $(sagarmatha_cflags)
sagarmatha_real_LDADD = libsagarmatha.la $(libsagarmatha_la_LIBADD)
sagarmatha_real_DEPENDENCIES = libsagarmatha.la

########################################

sagarmatha_recorder_sources =        \
	sagarmatha-recorder.c	\
	sagarmatha-recorder.h

# Custom element is an internal detail
sagarmatha_recorder_non_gir_sources =  \
	sagarmatha-recorder-src.c	  \
	sagarmatha-recorder-src.h

if BUILD_RECORDER
libsagarmatha_la_SOURCES += $(sagarmatha_recorder_sources) $(sagarmatha_recorder_non_gir_sources)

noinst_PROGRAMS += test-recorder

test_recorder_CPPFLAGS = $(TEST_CINNAMON_RECORDER_CFLAGS)
test_recorder_LDADD = $(TEST_CINNAMON_RECORDER_LIBS)

test_recorder_SOURCES =     \
	$(sagarmatha_recorder_sources) $(sagarmatha_recorder_non_gir_sources) \
	test-recorder.c
endif BUILD_RECORDER

########################################

pkglibexec_PROGRAMS += sagarmatha-perf-helper

sagarmatha_perf_helper_SOURCES = sagarmatha-perf-helper.c
sagarmatha_perf_helper_CPPFLAGS = $(CINNAMON_PERF_HELPER_CFLAGS)
sagarmatha_perf_helper_LDADD = $(CINNAMON_PERF_HELPER_LIBS)

########################################

noinst_PROGRAMS += run-js-test

run_js_test_CPPFLAGS = $(sagarmatha_cflags)
run_js_test_LDADD = libsagarmatha.la $(libsagarmatha_la_LIBADD)
run_js_test_LDFLAGS = -export-dynamic

run_js_test_SOURCES =			\
	run-js-test.c

########################################

sagarmatha-marshal.h: stamp-sagarmatha-marshal.h
	@true
stamp-sagarmatha-marshal.h: Makefile sagarmatha-marshal.list
	$(AM_V_GEN) $(GLIB_GENMARSHAL) \
		--prefix=_sagarmatha_marshal \
		--header \
	$(srcdir)/sagarmatha-marshal.list > xgen-smh && \
	(cmp -s xgen-smh sagarmatha-marshal.h || cp -f xgen-smh sagarmatha-marshal.h) && \
	rm -f xgen-smh && \
	echo timestamp > $(@F)
CLEANFILES += stamp-sagarmatha-marshal.h

sagarmatha-marshal.c: Makefile sagarmatha-marshal.list
	$(AM_V_GEN) (echo "#include \"sagarmatha-marshal.h\"" ; \
	 $(GLIB_GENMARSHAL) \
		--prefix=_sagarmatha_marshal \
		--body \
	 $(srcdir)/sagarmatha-marshal.list ) > xgen-smc && \
	cp -f xgen-smc sagarmatha-marshal.c && \
	rm -f xgen-smc


sagarmatha-enum-types.h: stamp-sagarmatha-enum-types.h Makefile
	@true
stamp-sagarmatha-enum-types.h: $(srcdir)/sagarmatha-enum-types.h.in $(sagarmatha_public_headers_h)
	$(AM_V_GEN) ( \
	  $(GLIB_MKENUMS) \
	    --template $< \
	  $(addprefix $(srcdir)/,$(sagarmatha_public_headers_h)) ) > $(@F).tmp && \
	(cmp -s $(@F).tmp sagarmatha-enum-types.h || mv $(@F).tmp sagarmatha-enum-types.h) && \
	rm -f $(@F).tmp && \
	echo timestamp > $(@F)
EXTRA_DIST += sagarmatha-enum-types.h.in
CLEANFILES += stamp-sagarmatha-enum-types.h

sagarmatha-enum-types.c: $(srcdir)/sagarmatha-enum-types.c.in stamp-sagarmatha-enum-types.h
	$(AM_V_GEN) ( \
	  $(GLIB_MKENUMS) \
	    --template $< \
	  $(addprefix $(srcdir)/,$(sagarmatha_public_headers_h)) ) > $(@F).tmp && \
	mv $(@F).tmp $(@F) && \
	rm -f $(@F).tmp
EXTRA_DIST += sagarmatha-enum-types.c.in

libsagarmatha_la_LDFLAGS = -avoid-version
libsagarmatha_la_LIBADD =		\
	-lm			\
	$(CINNAMON_LIBS)	\
	$(BLUETOOTH_LIBS)	\
	libst-1.0.la       	\
	libtray.la		\
	libgvc.la		\
	$(NULL)

libsagarmatha_la_CPPFLAGS = $(sagarmatha_cflags)

Sagarmatha-0.1.gir: libsagarmatha.la St-1.0.gir
Sagarmatha_0_1_gir_INCLUDES = Clutter-1.0 ClutterX11-1.0 Meta-Muffin.0 Soup-2.4 GMenu-3.0 NetworkManager-1.0 NMClient-1.0
Sagarmatha_0_1_gir_CFLAGS = $(libsagarmatha_la_CPPFLAGS) -I $(srcdir)
Sagarmatha_0_1_gir_LIBS = libsagarmatha.la
Sagarmatha_0_1_gir_FILES = $(libsagarmatha_la_gir_sources)
Sagarmatha_0_1_gir_SCANNERFLAGS = --include-uninstalled=$(builddir)/St-1.0.gir \
	--add-include-path=$(MUFFIN_GIR_DIR) $(if $(BLUETOOTH_DIR),-L $(BLUETOOTH_DIR),)
INTROSPECTION_GIRS += Sagarmatha-0.1.gir
CLEANFILES += Sagarmatha-0.1.gir

St-1.0.gir: libst-1.0.la
St_1_0_gir_INCLUDES = Clutter-1.0 Gtk-3.0
St_1_0_gir_CFLAGS = $(st_cflags) -DST_COMPILATION
St_1_0_gir_LIBS = libst-1.0.la
St_1_0_gir_FILES = $(filter-out %-private.h $(st_non_gir_sources), $(addprefix $(srcdir)/,$(st_source_h))) \
	$(addprefix $(srcdir)/,$(st_source_c))
INTROSPECTION_GIRS += St-1.0.gir
CLEANFILES += St-1.0.gir
