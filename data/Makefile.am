desktopdir=$(datadir)/applications
desktop_DATA = sagarmatha.desktop

# We substitute in bindir so it works as an autostart
# file when built in a non-system prefix
%.desktop.in:%.desktop.in.in
	$(AM_V_GEN) sed -e "s|@bindir[@]|$(bindir)|" \
	    -e "s|@VERSION[@]|$(VERSION)|" \
	    $< > $@ || rm $@

# Placeholder until we add intltool
%.desktop:%.desktop.in
	$(AM_V_GEN) sed s/^_// < $< > $@ || rm $@

searchprovidersdir = $(pkgdatadir)/search_providers
dist_searchproviders_DATA =				\
	search_providers/google.xml				\
	search_providers/wikipedia.xml

themedir = $(pkgdatadir)/theme
dist_theme_DATA = \
    theme/add-workspace-hover.png  \
    theme/add-workspace.png        \
    theme/calendar-arrow-left.svg  \
    theme/calendar-arrow-right.svg \
    theme/calendar-today.svg       \
    theme/checkbox-focused.svg     \
    theme/checkbox-off-focused.svg \
    theme/checkbox-off.svg         \
    theme/checkbox.svg             \
    theme/sagarmatha.css             \
    theme/close.svg                \
    theme/close-window.svg         \
    theme/corner-ripple.png        \
    theme/menu.png                 \
    theme/overview-hover.png       \
    theme/overview.png             \
    theme/player-paused.svg        \
    theme/player-playing.svg       \
    theme/player-stopped.svg       \
    theme/process-working.svg      \
    theme/radiobutton-off.svg      \
    theme/radiobutton.svg          \
    theme/scroll-hhandle.svg       \
    theme/scroll-vhandle.svg       \
    theme/thumbnail-generic.png    \
    theme/thumbnail.png            \
    theme/toggle-off-intl.svg      \
    theme/toggle-off-us.svg        \
    theme/toggle-on-intl.svg       \
    theme/toggle-on-us.svg         \
    theme/trash-icon.png

gsettings_SCHEMAS = org.sagarmatha.gschema.xml

@INTLTOOL_XML_NOMERGE_RULE@
@GSETTINGS_RULES@

# We need to compile schemas at make time
# to run from source tree
gschemas.compiled: $(gsettings_SCHEMAS:.xml=.valid)
	$(AM_V_GEN) $(GLIB_COMPILE_SCHEMAS) --targetdir=. .

all-local: gschemas.compiled


shadersdir = $(pkgdatadir)/shaders
shaders_DATA = \
	shaders/dim-window.glsl


EXTRA_DIST =						\
	sagarmatha.desktop.in.in			\
	$(menu_DATA)					\
	$(shaders_DATA)					\
	org.sagarmatha.gschema.xml.in

CLEANFILES =						\
	sagarmatha.desktop.in				\
	$(desktop_DATA)					\
	$(gsettings_SCHEMAS)				\
	gschemas.compiled

