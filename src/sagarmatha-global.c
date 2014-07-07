/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#include "config.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#include <X11/extensions/Xfixes.h>
#include <cogl-pango/cogl-pango.h>
#include <canberra.h>
#include <clutter/glx/clutter-glx.h>
#include <clutter/x11/clutter-x11.h>
#include <gdk/gdkx.h>
#include <gio/gio.h>
#include <cjs/gjs-module.h>
#include <girepository.h>
#include <meta/display.h>
#include <meta/util.h>

/* Memory report bits */
#ifdef HAVE_MALLINFO
#include <malloc.h>
#endif

#include "sagarmatha-enum-types.h"
#include "sagarmatha-global-private.h"
#include "sagarmatha-jsapi-compat-private.h"
#include "sagarmatha-marshal.h"
#include "sagarmatha-perf-log.h"
#include "sagarmatha-window-tracker.h"
#include "sagarmatha-wm.h"
#include "st.h"

static SagarmathaGlobal *the_object = NULL;

static void grab_notify (GtkWidget *widget, gboolean is_grab, gpointer user_data);
static void sagarmatha_global_on_gc (GjsContext   *context,
                                SagarmathaGlobal  *global);

struct _SagarmathaGlobal {
  GObject parent;

  ClutterStage *stage;
  Window stage_xwindow;
  GdkWindow *stage_gdk_window;

  MetaDisplay *meta_display;
  GdkDisplay *gdk_display;
  Display *xdisplay;
  MetaScreen *meta_screen;
  GdkScreen *gdk_screen;

  /* We use this window to get a notification from GTK+ when
   * a widget in our process does a GTK+ grab.  See
   * http://bugzilla.gnome.org/show_bug.cgi?id=570641
   * 
   * This window is never mapped or shown.
   */
  GtkWindow *grab_notifier;
  gboolean gtk_grab_active;

  SagarmathaStageInputMode input_mode;
  XserverRegion input_region;

  GjsContext *js_context;
  MetaPlugin *plugin;
  SagarmathaWM *wm;
  GSettings *settings;
  const char *datadir;
  const char *imagedir;
  const char *userdatadir;
  StFocusManager *focus_manager;

  guint work_count;
  GSList *leisure_closures;
  guint leisure_function_id;

  /* For sound notifications */
  ca_context *sound_context;

  guint32 xdnd_timestamp;
  gint64 last_gc_end_time;
};

enum {
  PROP_0,

  PROP_OVERLAY_GROUP,
  PROP_SCREEN,
  PROP_GDK_SCREEN,
  PROP_DISPLAY,
  PROP_SCREEN_WIDTH,
  PROP_SCREEN_HEIGHT,
  PROP_STAGE,
  PROP_STAGE_INPUT_MODE,
  PROP_BOTTOM_WINDOW_GROUP,
  PROP_WINDOW_GROUP,
  PROP_TOP_WINDOW_GROUP,
  PROP_BACKGROUND_ACTOR,
  PROP_WINDOW_MANAGER,
  PROP_SETTINGS,
  PROP_DATADIR,
  PROP_IMAGEDIR,
  PROP_USERDATADIR,
  PROP_FOCUS_MANAGER,
};

/* Signals */
enum
{
 XDND_POSITION_CHANGED,
 XDND_LEAVE,
 XDND_ENTER,
 NOTIFY_ERROR,
 LAST_SIGNAL
};

G_DEFINE_TYPE(SagarmathaGlobal, sagarmatha_global, G_TYPE_OBJECT);

static guint sagarmatha_global_signals [LAST_SIGNAL] = { 0 };

static void
sagarmatha_global_set_property(GObject         *object,
                          guint            prop_id,
                          const GValue    *value,
                          GParamSpec      *pspec)
{
  SagarmathaGlobal *global = SAGARMATHA_GLOBAL (object);

  switch (prop_id)
    {
    case PROP_STAGE_INPUT_MODE:
      sagarmatha_global_set_stage_input_mode (global, g_value_get_enum (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
sagarmatha_global_get_property(GObject         *object,
                          guint            prop_id,
                          GValue          *value,
                          GParamSpec      *pspec)
{
  SagarmathaGlobal *global = SAGARMATHA_GLOBAL (object);

  switch (prop_id)
    {
    case PROP_OVERLAY_GROUP:
      g_value_set_object (value, meta_get_overlay_group_for_screen (global->meta_screen));
      break;
    case PROP_SCREEN:
      g_value_set_object (value, global->meta_screen);
      break;
    case PROP_GDK_SCREEN:
      g_value_set_object (value, global->gdk_screen);
      break;
    case PROP_DISPLAY:
      g_value_set_object (value, global->meta_display);
      break;
    case PROP_SCREEN_WIDTH:
      {
        int width, height;

        meta_screen_get_size (global->meta_screen, &width, &height);
        g_value_set_int (value, width);
      }
      break;
    case PROP_SCREEN_HEIGHT:
      {
        int width, height;

        meta_screen_get_size (global->meta_screen, &width, &height);
        g_value_set_int (value, height);
      }
      break;
    case PROP_STAGE:
      g_value_set_object (value, global->stage);
      break;
    case PROP_STAGE_INPUT_MODE:
      g_value_set_enum (value, global->input_mode);
      break;
    case PROP_BOTTOM_WINDOW_GROUP:
      g_value_set_object (value, meta_get_bottom_window_group_for_screen (global->meta_screen));
      break;
    case PROP_WINDOW_GROUP:
      g_value_set_object (value, meta_get_window_group_for_screen (global->meta_screen));
      break;
    case PROP_TOP_WINDOW_GROUP:
      g_value_set_object (value, meta_get_top_window_group_for_screen (global->meta_screen));
      break;
    case PROP_BACKGROUND_ACTOR:
      g_value_set_object (value, meta_get_background_actor_for_screen (global->meta_screen));
      break;
    case PROP_WINDOW_MANAGER:
      g_value_set_object (value, global->wm);
      break;
    case PROP_SETTINGS:
      g_value_set_object (value, global->settings);
      break;
    case PROP_DATADIR:
      g_value_set_string (value, global->datadir);
      break;
    case PROP_IMAGEDIR:
      g_value_set_string (value, global->imagedir);
      break;
    case PROP_USERDATADIR:
      g_value_set_string (value, global->userdatadir);
      break;
    case PROP_FOCUS_MANAGER:
      g_value_set_object (value, global->focus_manager);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
sagarmatha_global_init (SagarmathaGlobal *global)
{
  const char *datadir = g_getenv ("SAGARMATHA_DATADIR");
  const char *sagarmatha_js = g_getenv("SAGARMATHA_JS");
  char *imagedir, **search_path;

  if (!datadir)
    datadir = SAGARMATHA_DATADIR;
  global->datadir = datadir;

  /* We make sure imagedir ends with a '/', since the JS won't have
   * access to g_build_filename() and so will end up just
   * concatenating global.imagedir to a filename.
   */
  imagedir = g_build_filename (datadir, "images/", NULL);
  if (g_file_test (imagedir, G_FILE_TEST_IS_DIR))
    global->imagedir = imagedir;
  else
    {
      g_free (imagedir);
      global->imagedir = g_strdup_printf ("%s/", datadir);
    }

  /* Ensure config dir exists for later use */
  global->userdatadir = g_build_filename (g_get_user_data_dir (), "sagarmatha", NULL);
  g_mkdir_with_parents (global->userdatadir, 0700);

  global->settings = g_settings_new ("org.sagarmatha");
  
  global->grab_notifier = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
  g_signal_connect (global->grab_notifier, "grab-notify", G_CALLBACK (grab_notify), global);
  global->gtk_grab_active = FALSE;

  global->input_mode = SAGARMATHA_STAGE_INPUT_MODE_NORMAL;

  ca_context_create (&global->sound_context);
  ca_context_change_props (global->sound_context, CA_PROP_APPLICATION_NAME, PACKAGE_NAME, CA_PROP_APPLICATION_ID, "org.Sagarmatha", NULL);
  ca_context_open (global->sound_context);

  if (!sagarmatha_js)
    sagarmatha_js = JSDIR;
  search_path = g_strsplit (sagarmatha_js, ":", -1);
  global->js_context = g_object_new (GJS_TYPE_CONTEXT,
                                     "search-path", search_path,
                                     "js-version", "1.8",
                                     "gc-notifications", TRUE,
                                     NULL);
  g_signal_connect (global->js_context, "gc", G_CALLBACK (sagarmatha_global_on_gc), global);

  g_strfreev (search_path);
}

static void
sagarmatha_global_finalize (GObject *object)
{
  SagarmathaGlobal *global = SAGARMATHA_GLOBAL (object);

  g_object_unref (global->js_context);
  gtk_widget_destroy (GTK_WIDGET (global->grab_notifier));
  g_object_unref (global->settings);

  the_object = NULL;

  G_OBJECT_CLASS(sagarmatha_global_parent_class)->finalize (object);
}

static void
sagarmatha_global_class_init (SagarmathaGlobalClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->get_property = sagarmatha_global_get_property;
  gobject_class->set_property = sagarmatha_global_set_property;
  gobject_class->finalize = sagarmatha_global_finalize;

  /* Emitted from sagarmatha-plugin.c during event handling */
  sagarmatha_global_signals[XDND_POSITION_CHANGED] =
      g_signal_new ("xdnd-position-changed",
                    G_TYPE_FROM_CLASS (klass),
                    G_SIGNAL_RUN_LAST,
                    0,
                    NULL, NULL,
                    _sagarmatha_marshal_VOID__INT_INT,
                    G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);

  /* Emitted from sagarmatha-plugin.c during event handling */
  sagarmatha_global_signals[XDND_LEAVE] =
      g_signal_new ("xdnd-leave",
                    G_TYPE_FROM_CLASS (klass),
                    G_SIGNAL_RUN_LAST,
                    0,
                    NULL, NULL,
                    g_cclosure_marshal_VOID__VOID,
                    G_TYPE_NONE, 0);

  /* Emitted from sagarmatha-plugin.c during event handling */
  sagarmatha_global_signals[XDND_ENTER] =
      g_signal_new ("xdnd-enter",
                    G_TYPE_FROM_CLASS (klass),
                    G_SIGNAL_RUN_LAST,
                    0,
                    NULL, NULL,
                    g_cclosure_marshal_VOID__VOID,
                    G_TYPE_NONE, 0);

  sagarmatha_global_signals[NOTIFY_ERROR] =
      g_signal_new ("notify-error",
                    G_TYPE_FROM_CLASS (klass),
                    G_SIGNAL_RUN_LAST,
                    0,
                    NULL, NULL,
                    gi_cclosure_marshal_generic,
                    G_TYPE_NONE, 2,
                    G_TYPE_STRING,
                    G_TYPE_STRING);

  g_object_class_install_property (gobject_class,
                                   PROP_OVERLAY_GROUP,
                                   g_param_spec_object ("overlay-group",
                                                        "Overlay Group",
                                                        "Actor holding objects that appear above the desktop contents",
                                                        CLUTTER_TYPE_ACTOR,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_SCREEN,
                                   g_param_spec_object ("screen",
                                                        "Screen",
                                                        "Metacity screen object for Sagarmatha",
                                                        META_TYPE_SCREEN,
                                                        G_PARAM_READABLE));

  g_object_class_install_property (gobject_class,
                                   PROP_GDK_SCREEN,
                                   g_param_spec_object ("gdk-screen",
                                                        "GdkScreen",
                                                        "Gdk screen object for Sagarmatha",
                                                        GDK_TYPE_SCREEN,
                                                        G_PARAM_READABLE));

  g_object_class_install_property (gobject_class,
                                   PROP_SCREEN_WIDTH,
                                   g_param_spec_int ("screen-width",
                                                     "Screen Width",
                                                     "Screen width, in pixels",
                                                     0, G_MAXINT, 1,
                                                     G_PARAM_READABLE));

  g_object_class_install_property (gobject_class,
                                   PROP_SCREEN_HEIGHT,
                                   g_param_spec_int ("screen-height",
                                                     "Screen Height",
                                                     "Screen height, in pixels",
                                                     0, G_MAXINT, 1,
                                                     G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_DISPLAY,
                                   g_param_spec_object ("display",
                                                        "Display",
                                                        "Metacity display object for Sagarmatha",
                                                        META_TYPE_DISPLAY,
                                                        G_PARAM_READABLE));

  g_object_class_install_property (gobject_class,
                                   PROP_STAGE,
                                   g_param_spec_object ("stage",
                                                        "Stage",
                                                        "Stage holding the desktop scene graph",
                                                        CLUTTER_TYPE_ACTOR,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_STAGE_INPUT_MODE,
                                   g_param_spec_enum ("stage-input-mode",
                                                      "Stage input mode",
                                                      "The stage input mode",
                                                      SAGARMATHA_TYPE_STAGE_INPUT_MODE,
                                                      SAGARMATHA_STAGE_INPUT_MODE_NORMAL,
                                                      G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_BOTTOM_WINDOW_GROUP,
                                   g_param_spec_object ("bottom-window-group",
                                                        "Bottom Window Group",
                                                        "Actor holding window actors that must appear below desklets",
                                                        CLUTTER_TYPE_ACTOR,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_WINDOW_GROUP,
                                   g_param_spec_object ("window-group",
                                                        "Window Group",
                                                        "Actor holding window actors",
                                                        CLUTTER_TYPE_ACTOR,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_TOP_WINDOW_GROUP,
                                   g_param_spec_object ("top-window-group",
                                                        "Top Window Group",
                                                        "Actor holding popup menus and other actors which must appear on top of the panels",
                                                        CLUTTER_TYPE_ACTOR,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_BACKGROUND_ACTOR,
                                   g_param_spec_object ("background-actor",
                                                        "Background Actor",
                                                        "Actor drawing root window background",
                                                        CLUTTER_TYPE_ACTOR,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_WINDOW_MANAGER,
                                   g_param_spec_object ("window-manager",
                                                        "Window Manager",
                                                        "Window management interface",
                                                        SAGARMATHA_TYPE_WM,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_SETTINGS,
                                   g_param_spec_object ("settings",
                                                        "Settings",
                                                        "GSettings instance for sagarmatha configuration",
                                                        G_TYPE_SETTINGS,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_DATADIR,
                                   g_param_spec_string ("datadir",
                                                        "Data directory",
                                                        "Directory containing sagarmatha data files",
                                                        NULL,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_IMAGEDIR,
                                   g_param_spec_string ("imagedir",
                                                        "Image directory",
                                                        "Directory containing sagarmatha image files",
                                                        NULL,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_USERDATADIR,
                                   g_param_spec_string ("userdatadir",
                                                        "User data directory",
                                                        "Directory containing sagarmatha user data",
                                                        NULL,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_FOCUS_MANAGER,
                                   g_param_spec_object ("focus-manager",
                                                        "Focus manager",
                                                        "Sagarmatha's StFocusManager",
                                                        ST_TYPE_FOCUS_MANAGER,
                                                        G_PARAM_READABLE));
}

/**•
 * _sagarmatha_global_init: (skip)•
 * @first_property_name: the name of the first property
 * @...: the value of the first property, followed optionally by more
 *  name/value pairs, followed by %NULL
 *•
 * Initializes Sagarmatha global singleton with the construction-time
 * properties.
 *
 * There are currently no such properties, so @first_property_name should
 * always be %NULL.
 *
 * This call must be called before sagarmatha_global_get() and shouldn't be called
 * more than once.
 */
void
_sagarmatha_global_init (const char *first_property_name,
                    ...)
{
  va_list argument_list;

  g_return_if_fail (the_object == NULL);

  va_start (argument_list, first_property_name);
  the_object = SAGARMATHA_GLOBAL (g_object_new_valist (SAGARMATHA_TYPE_GLOBAL,
                                                  first_property_name,
                                                  argument_list));
  va_end (argument_list);

}

/**
 * sagarmatha_global_get:
 *
 * Gets the singleton global object that represents the desktop.
 *
 * Return value: (transfer none): the singleton global object
 */
SagarmathaGlobal *
sagarmatha_global_get (void)
{
  return the_object;
}

static void
focus_window_changed (MetaDisplay *display,
                      GParamSpec  *param,
                      gpointer     user_data)
{
  SagarmathaGlobal *global = user_data;

  if (global->input_mode == SAGARMATHA_STAGE_INPUT_MODE_FOCUSED &&
      meta_display_get_focus_window (display) != NULL)
    sagarmatha_global_set_stage_input_mode (global, SAGARMATHA_STAGE_INPUT_MODE_NORMAL);
}

static void
sagarmatha_global_focus_stage (SagarmathaGlobal *global)
{
  XSetInputFocus (global->xdisplay, global->stage_xwindow,
                  RevertToPointerRoot,
                  sagarmatha_global_get_current_time (global));
}

/**
 * sagarmatha_global_set_stage_input_mode:
 * @global: the #SagarmathaGlobal
 * @mode: the stage input mode
 *
 * Sets the input mode of the stage; when @mode is
 * %SAGARMATHA_STAGE_INPUT_MODE_NONREACTIVE, then the stage does not absorb
 * any clicks, but just passes them through to underlying windows.
 * When it is %SAGARMATHA_STAGE_INPUT_MODE_NORMAL, then the stage accepts
 * clicks in the region defined by
 * sagarmatha_global_set_stage_input_region() but passes through clicks
 * outside that region. When it is %SAGARMATHA_STAGE_INPUT_MODE_FULLSCREEN,
 * the stage absorbs all input.
 *
 * When the input mode is %SAGARMATHA_STAGE_INPUT_MODE_FOCUSED, the pointer
 * is handled as with %SAGARMATHA_STAGE_INPUT_MODE_NORMAL, but additionally
 * the stage window has the keyboard focus. If the stage loses the
 * focus (eg, because the user clicked into a window) the input mode
 * will revert to %SAGARMATHA_STAGE_INPUT_MODE_NORMAL.
 *
 * Note that whenever a muffin-internal Gtk widget has a pointer grab,
 * Sagarmatha behaves as though it was in
 * %SAGARMATHA_STAGE_INPUT_MODE_NONREACTIVE, to ensure that the widget gets
 * any clicks it is expecting.
 */
void
sagarmatha_global_set_stage_input_mode (SagarmathaGlobal         *global,
                                   SagarmathaStageInputMode  mode)
{
  MetaScreen *screen;

  g_return_if_fail (SAGARMATHA_IS_GLOBAL (global));

  screen = meta_plugin_get_screen (global->plugin);

  if (mode == SAGARMATHA_STAGE_INPUT_MODE_NONREACTIVE || global->gtk_grab_active)
    meta_empty_stage_input_region (screen);
  else if (mode == SAGARMATHA_STAGE_INPUT_MODE_FULLSCREEN || !global->input_region)
    meta_set_stage_input_region (screen, None);
  else
    meta_set_stage_input_region (screen, global->input_region);

  if (mode == SAGARMATHA_STAGE_INPUT_MODE_FOCUSED)
    sagarmatha_global_focus_stage (global);

  if (mode != global->input_mode)
    {
      global->input_mode = mode;
      g_object_notify (G_OBJECT (global), "stage-input-mode");
    }
}

/**
 * sagarmatha_global_set_cursor:
 * @global: A #SagarmathaGlobal
 * @type: the type of the cursor
 *
 * Set the cursor on the stage window.
 */
void
sagarmatha_global_set_cursor (SagarmathaGlobal *global,
                         SagarmathaCursor type)
{
  const char *name;
  GdkCursor *cursor;

  switch (type)
    {
    case SAGARMATHA_CURSOR_DND_IN_DRAG:
      name = "dnd-none";
      break;
    case SAGARMATHA_CURSOR_DND_MOVE:
      name = "dnd-move";
      break;
    case SAGARMATHA_CURSOR_DND_COPY:
      name = "dnd-copy";
      break;
    case SAGARMATHA_CURSOR_DND_UNSUPPORTED_TARGET:
      name = "dnd-none";
      break;
    case SAGARMATHA_CURSOR_POINTING_HAND:
      name = "hand";
      break;
    default:
      g_return_if_reached ();
    }

  cursor = gdk_cursor_new_from_name (global->gdk_display, name);
  if (!cursor)
    {
      GdkCursorType cursor_type;
      switch (type)
        {
        case SAGARMATHA_CURSOR_DND_IN_DRAG:
          cursor_type = GDK_FLEUR;
          break;
        case SAGARMATHA_CURSOR_DND_MOVE:
          cursor_type = GDK_TARGET;
          break;
        case SAGARMATHA_CURSOR_DND_COPY:
          cursor_type = GDK_PLUS;
          break;
        case SAGARMATHA_CURSOR_POINTING_HAND:
          cursor_type = GDK_HAND2;
          break;
        case SAGARMATHA_CURSOR_DND_UNSUPPORTED_TARGET:
          cursor_type = GDK_X_CURSOR;
          break;
        default:
          g_return_if_reached ();
        }
      cursor = gdk_cursor_new (cursor_type);
    }

  gdk_window_set_cursor (global->stage_gdk_window, cursor);

  g_object_unref (cursor);
}

/**
 * sagarmatha_global_unset_cursor:
 * @global: A #SagarmathaGlobal
 *
 * Unset the cursor on the stage window.
 */
void
sagarmatha_global_unset_cursor (SagarmathaGlobal  *global)
{
  gdk_window_set_cursor (global->stage_gdk_window, NULL);
}

/**
 * sagarmatha_global_set_stage_input_region:
 * @global: the #SagarmathaGlobal
 * @rectangles: (element-type Meta.Rectangle): a list of #MetaRectangle
 * describing the input region.
 *
 * Sets the area of the stage that is responsive to mouse clicks when
 * the stage mode is %SAGARMATHA_STAGE_INPUT_MODE_NORMAL (but does not change the
 * current stage mode).
 */
void
sagarmatha_global_set_stage_input_region (SagarmathaGlobal *global,
                                     GSList      *rectangles)
{
  MetaRectangle *rect;
  XRectangle *rects;
  int nrects, i;
  GSList *r;

  g_return_if_fail (SAGARMATHA_IS_GLOBAL (global));

  nrects = g_slist_length (rectangles);
  rects = g_new (XRectangle, nrects);
  for (r = rectangles, i = 0; r; r = r->next, i++)
    {
      rect = (MetaRectangle *)r->data;
      rects[i].x = rect->x;
      rects[i].y = rect->y;
      rects[i].width = rect->width;
      rects[i].height = rect->height;
    }

  if (global->input_region)
    XFixesDestroyRegion (global->xdisplay, global->input_region);

  global->input_region = XFixesCreateRegion (global->xdisplay, rects, nrects);
  g_free (rects);

  /* set_stage_input_mode() will figure out whether or not we
   * should actually change the input region right now.
   */
  sagarmatha_global_set_stage_input_mode (global, global->input_mode);
}

/**
 * sagarmatha_global_get_stage:
 *
 * Return value: (transfer none): The default #ClutterStage
 */
ClutterStage *
sagarmatha_global_get_stage (SagarmathaGlobal  *global)
{
  return global->stage;
}

/**
 * sagarmatha_global_get_screen:
 *
 * Return value: (transfer none): The default #MetaScreen
 */
MetaScreen *
sagarmatha_global_get_screen (SagarmathaGlobal  *global)
{
  return global->meta_screen;
}

/**
 * sagarmatha_global_get_gdk_screen:
 *
 * Return value: (transfer none): Gdk screen object for Sagarmatha
 */
GdkScreen *
sagarmatha_global_get_gdk_screen (SagarmathaGlobal *global)
{
  g_return_val_if_fail (SAGARMATHA_IS_GLOBAL (global), NULL);

  return global->gdk_screen;
}

/**
 * sagarmatha_global_get_display:
 *
 * Return value: (transfer none): The default #MetaDisplay
 */
MetaDisplay *
sagarmatha_global_get_display (SagarmathaGlobal  *global)
{
  return global->meta_display;
}

/**
 * sagarmatha_global_get_window_actors:
 *
 * Gets the list of #MetaWindowActor for the plugin's screen
 *
 * Return value: (element-type Meta.WindowActor) (transfer none): the list of windows
 */
GList *
sagarmatha_global_get_window_actors (SagarmathaGlobal *global)
{
  g_return_val_if_fail (SAGARMATHA_IS_GLOBAL (global), NULL);

  return meta_get_window_actors (global->meta_screen);
}

static void
global_stage_notify_width (GObject    *gobject,
                           GParamSpec *pspec,
                           gpointer    data)
{
  SagarmathaGlobal *global = SAGARMATHA_GLOBAL (data);

  g_object_notify (G_OBJECT (global), "screen-width");
}

static void
global_stage_notify_height (GObject    *gobject,
                            GParamSpec *pspec,
                            gpointer    data)
{
  SagarmathaGlobal *global = SAGARMATHA_GLOBAL (data);

  g_object_notify (G_OBJECT (global), "screen-height");
}

static void
global_stage_before_paint (ClutterStage *stage,
                           SagarmathaGlobal  *global)
{
  sagarmatha_perf_log_event (sagarmatha_perf_log_get_default (),
                        "clutter.stagePaintStart");
}

static void
global_stage_after_paint (ClutterStage *stage,
                          SagarmathaGlobal  *global)
{
  sagarmatha_perf_log_event (sagarmatha_perf_log_get_default (),
                        "clutter.stagePaintDone");
}

static void
constrain_tooltip (StTooltip             *tooltip,
                   const ClutterGeometry *geometry,
                   ClutterGeometry       *adjusted_geometry,
                   gpointer               data)
{
  const ClutterGeometry *tip_area = st_tooltip_get_tip_area (tooltip);
  SagarmathaGlobal *global = sagarmatha_global_get ();
  MetaScreen *screen = sagarmatha_global_get_screen (global);
  int n_monitors = meta_screen_get_n_monitors (screen);
  int i;

  *adjusted_geometry = *geometry;

  /* A point that determines what screen we'll constrain to */
  int x = tip_area->x + tip_area->width / 2;
  int y = tip_area->y + tip_area->height / 2;

  for (i = 0; i < n_monitors; i++)
    {
      MetaRectangle rect;
      meta_screen_get_monitor_geometry (screen, i, &rect);
      if (x >= rect.x && x < rect.x + rect.width &&
          y >= rect.y && y < rect.y + rect.height)
        {
          if (adjusted_geometry->x + adjusted_geometry->width > rect.x + rect.width)
            adjusted_geometry->x = rect.x + rect.width - adjusted_geometry->width;
          if (adjusted_geometry->x < rect.x)
            adjusted_geometry->x = rect.x;

          if (adjusted_geometry->y + adjusted_geometry->height > rect.y + rect.height)
            adjusted_geometry->y = rect.y + rect.height - adjusted_geometry->height;
          if (adjusted_geometry->y < rect.y)
            adjusted_geometry->y = rect.y;

          return;
        }
    }
}

static void
update_font_options (GtkSettings  *settings,
                     ClutterStage *stage)
{
  StThemeContext *context;
  ClutterBackend *backend;
  gint dpi;
  gint hinting;
  gchar *hint_style_str;
  cairo_hint_style_t hint_style = CAIRO_HINT_STYLE_NONE;
  gint antialias;
  cairo_antialias_t antialias_mode = CAIRO_ANTIALIAS_NONE;
  cairo_font_options_t *options;

  g_object_get (settings,
                "gtk-xft-dpi", &dpi,
                "gtk-xft-antialias", &antialias,
                "gtk-xft-hinting", &hinting,
                "gtk-xft-hintstyle", &hint_style_str,
                NULL);

  context = st_theme_context_get_for_stage (stage);

  if (dpi != -1)
    /* GTK stores resolution as 1024 * dots/inch */
    st_theme_context_set_resolution (context, dpi / 1024);
  else
    st_theme_context_set_default_resolution (context);

  st_tooltip_set_constrain_func (stage, constrain_tooltip, NULL, NULL);

  /* Clutter (as of 0.9) passes comprehensively wrong font options
   * override whatever set_font_flags() did above.
   *
   * http://bugzilla.openedhand.com/show_bug.cgi?id=1456
   */
  backend = clutter_get_default_backend ();
  options = cairo_font_options_create ();

  cairo_font_options_set_hint_metrics (options, CAIRO_HINT_METRICS_ON);

  if (hinting >= 0 && !hinting)
    {
      hint_style = CAIRO_HINT_STYLE_NONE;
    }
  else if (hint_style_str)
    {
      if (strcmp (hint_style_str, "hintnone") == 0)
        hint_style = CAIRO_HINT_STYLE_NONE;
      else if (strcmp (hint_style_str, "hintslight") == 0)
        hint_style = CAIRO_HINT_STYLE_SLIGHT;
      else if (strcmp (hint_style_str, "hintmedium") == 0)
        hint_style = CAIRO_HINT_STYLE_MEDIUM;
      else if (strcmp (hint_style_str, "hintfull") == 0)
        hint_style = CAIRO_HINT_STYLE_FULL;
    }

  g_free (hint_style_str);

  cairo_font_options_set_hint_style (options, hint_style);

  /* We don't want to turn on subpixel anti-aliasing; since Clutter
   * doesn't currently have the code to support ARGB masks,
   * generating them then squashing them back to A8 is pointless.
   */
  antialias_mode = (antialias < 0 || antialias) ? CAIRO_ANTIALIAS_GRAY
                                                : CAIRO_ANTIALIAS_NONE;

  cairo_font_options_set_antialias (options, antialias_mode);

  clutter_backend_set_font_options (backend, options);
  cairo_font_options_destroy (options);
}

static void
settings_notify_cb (GtkSettings *settings,
                    GParamSpec  *pspec,
                    gpointer     data)
{
  update_font_options (settings, CLUTTER_STAGE (data));
}

static void
sagarmatha_fonts_init (ClutterStage *stage)
{
  GtkSettings *settings;
  CoglPangoFontMap *fontmap;

  /* Disable text mipmapping; it causes problems on pre-GEM Intel
   * drivers and we should just be rendering text at the right
   * size rather than scaling it. If we do effects where we dynamically
   * zoom labels, then we might want to reconsider.
   */
  fontmap = COGL_PANGO_FONT_MAP (clutter_get_font_map ());
  cogl_pango_font_map_set_use_mipmapping (fontmap, FALSE);

  settings = gtk_settings_get_default ();
  g_object_connect (settings,
                    "signal::notify::gtk-xft-dpi",
                    G_CALLBACK (settings_notify_cb), stage,
                    "signal::notify::gtk-xft-antialias",
                    G_CALLBACK (settings_notify_cb), stage,
                    "signal::notify::gtk-xft-hinting",
                    G_CALLBACK (settings_notify_cb), stage,
                    "signal::notify::gtk-xft-hintstyle",
                    G_CALLBACK (settings_notify_cb), stage,
                    NULL);
  update_font_options (settings, stage);
}

/* This is an IBus workaround. The flow of events with IBus is that every time
 * it gets gets a key event, it:
 *
 *  Sends it to the daemon via D-Bus asynchronously
 *  When it gets an reply, synthesizes a new GdkEvent and puts it into the
 *   GDK event queue with gdk_event_put(), including
 *   IBUS_FORWARD_MASK = 1 << 25 in the state to prevent a loop.
 *
 * (Normally, IBus uses the GTK+ key snooper mechanism to get the key
 * events early, but since our key events aren't visible to GTK+ key snoopers,
 * IBus will instead get the events via the standard
 * GtkIMContext.filter_keypress() mechanism.)
 *
 * There are a number of potential problems here; probably the worst
 * problem is that IBus doesn't forward the timestamp with the event
 * so that every key event that gets delivered ends up with
 * GDK_CURRENT_TIME.  This creates some very subtle bugs; for example
 * if you have IBus running and a keystroke is used to trigger
 * launching an application, focus stealing prevention won't work
 * right. http://code.google.com/p/ibus/issues/detail?id=1184
 *
 * In any case, our normal flow of key events is:
 *
 *  GDK filter function => clutter_x11_handle_event => clutter actor
 *
 * So, if we see a key event that gets delivered via the GDK event handler
 * function - then we know it must be one of these synthesized events, and
 * we should push it back to clutter.
 *
 * To summarize, the full key event flow with IBus is:
 *
 *   GDK filter function
 *     => Mutter
 *     => gnome_sagarmatha_plugin_xevent_filter()
 *     => clutter_x11_handle_event()
 *     => clutter event delivery to actor
 *     => gtk_im_context_filter_event()
 *     => sent to IBus daemon
 *     => response received from IBus daemon
 *     => gdk_event_put()
 *     => GDK event handler
 *     => <this function>
 *     => clutter_event_put()
 *     => clutter event delivery to actor
 *
 * Anything else we see here we just pass on to the normal GDK event handler
 * gtk_main_do_event().
 */
static void
gnome_sagarmatha_gdk_event_handler (GdkEvent *event_gdk,
                               gpointer  data)
{
  if (event_gdk->type == GDK_KEY_PRESS || event_gdk->type == GDK_KEY_RELEASE)
    {
      ClutterActor *stage;
      Window stage_xwindow;

      stage = CLUTTER_ACTOR (data);
      stage_xwindow = clutter_x11_get_stage_window (CLUTTER_STAGE (stage));

      if (GDK_WINDOW_XID (event_gdk->key.window) == stage_xwindow)
        {
          ClutterDeviceManager *device_manager = clutter_device_manager_get_default ();
          ClutterInputDevice *keyboard = clutter_device_manager_get_core_device (device_manager,
                                                                                 CLUTTER_KEYBOARD_DEVICE);

          ClutterEvent *event_clutter = clutter_event_new ((event_gdk->type == GDK_KEY_PRESS) ?
                                                           CLUTTER_KEY_PRESS : CLUTTER_KEY_RELEASE);
          event_clutter->key.time = event_gdk->key.time;
          event_clutter->key.flags = CLUTTER_EVENT_NONE;
          event_clutter->key.stage = CLUTTER_STAGE (stage);
          event_clutter->key.source = NULL;

          /* This depends on ClutterModifierType and GdkModifierType being
           * identical, which they are currently. (They both match the X
           * modifier state in the low 16-bits and have the same extensions.) */
          event_clutter->key.modifier_state = event_gdk->key.state;

          event_clutter->key.keyval = event_gdk->key.keyval;
          event_clutter->key.hardware_keycode = event_gdk->key.hardware_keycode;
          event_clutter->key.unicode_value = gdk_keyval_to_unicode (event_clutter->key.keyval);
          event_clutter->key.device = keyboard;

          clutter_event_put (event_clutter);
          clutter_event_free (event_clutter);

          return;
        }
    }

  gtk_main_do_event (event_gdk);
}

void
_sagarmatha_global_set_plugin (SagarmathaGlobal *global,
                          MetaPlugin  *plugin)
{
  g_return_if_fail (SAGARMATHA_IS_GLOBAL (global));
  g_return_if_fail (global->plugin == NULL);

  global->plugin = plugin;
  global->wm = sagarmatha_wm_new (plugin);

  global->meta_screen = meta_plugin_get_screen (plugin);
  global->meta_display = meta_screen_get_display (global->meta_screen);
  global->xdisplay = meta_display_get_xdisplay (global->meta_display);

  global->gdk_display = gdk_x11_lookup_xdisplay (global->xdisplay);
  global->gdk_screen = gdk_display_get_screen (global->gdk_display,
                                               meta_screen_get_screen_number (global->meta_screen));

  global->stage = CLUTTER_STAGE (meta_get_stage_for_screen (global->meta_screen));
  global->stage_xwindow = clutter_x11_get_stage_window (global->stage);
  global->stage_gdk_window = gdk_x11_window_foreign_new_for_display (global->gdk_display,
                                                                     global->stage_xwindow);

  g_signal_connect (global->stage, "notify::width",
                    G_CALLBACK (global_stage_notify_width), global);
  g_signal_connect (global->stage, "notify::height",
                    G_CALLBACK (global_stage_notify_height), global);

  g_signal_connect (global->stage, "paint",
                    G_CALLBACK (global_stage_before_paint), global);
  g_signal_connect_after (global->stage, "paint",
                          G_CALLBACK (global_stage_after_paint), global);

  sagarmatha_perf_log_define_event (sagarmatha_perf_log_get_default(),
                               "clutter.stagePaintStart",
                               "Start of stage page repaint",
                               "");
  sagarmatha_perf_log_define_event (sagarmatha_perf_log_get_default(),
                               "clutter.stagePaintDone",
                               "End of stage page repaint",
                               "");

  g_signal_connect (global->meta_display, "notify::focus-window",
                    G_CALLBACK (focus_window_changed), global);

  sagarmatha_fonts_init (global->stage);

  gdk_event_handler_set (gnome_sagarmatha_gdk_event_handler, global->stage, NULL);

  global->focus_manager = st_focus_manager_get_for_stage (global->stage);
}

GjsContext *
_sagarmatha_global_get_gjs_context (SagarmathaGlobal *global)
{
  return global->js_context;
}

/**
 * sagarmatha_global_begin_modal:
 * @global: a #SagarmathaGlobal
 *
 * Grabs the keyboard and mouse to the stage window. The stage will
 * receive all keyboard and mouse events until sagarmatha_global_end_modal()
 * is called. This is used to implement "modes" for Sagarmatha, such as the
 * overview mode or the "looking glass" debug overlay, that block
 * application and normal key shortcuts.
 *
 * Returns value: %TRUE if we succesfully entered the mode. %FALSE if we couldn't
 *  enter the mode. Failure may occur because an application has the pointer
 *  or keyboard grabbed, because Muffin is in a mode itself like moving a
 *  window or alt-Tab window selection, or because sagarmatha_global_begin_modal()
 *  was previouly called.
 */
gboolean
sagarmatha_global_begin_modal (SagarmathaGlobal *global,
                          guint32      timestamp)
{
  return meta_plugin_begin_modal (global->plugin, global->stage_xwindow,
                                  None, 0, timestamp);
}

/**
 * sagarmatha_global_end_modal:
 * @global: a #SagarmathaGlobal
 *
 * Undoes the effect of sagarmatha_global_begin_modal().
 */
void
sagarmatha_global_end_modal (SagarmathaGlobal *global,
                        guint32      timestamp)
{
  meta_plugin_end_modal (global->plugin, timestamp);
}

/**
 * sagarmatha_global_create_pointer_barrier:
 * @global: a #SagarmathaGlobal
 * @x1: left X coordinate
 * @y1: top Y coordinate
 * @x2: right X coordinate
 * @y2: bottom Y coordinate
 * @directions: The directions we're allowed to pass through
 *
 * If supported by X creates a pointer barrier.
 *
 * Return value: value you can pass to sagarmatha_global_destroy_pointer_barrier()
 */
guint32
sagarmatha_global_create_pointer_barrier (SagarmathaGlobal *global,
                                     int x1, int y1, int x2, int y2,
                                     int directions)
{
#if HAVE_XFIXESCREATEPOINTERBARRIER
  return (guint32)
    XFixesCreatePointerBarrier (global->xdisplay,
                                DefaultRootWindow (global->xdisplay),
                                x1, y1,
                                x2, y2,
                                directions,
                                0, NULL);
#else
  return 0;
#endif
}

/**
 * sagarmatha_global_destroy_pointer_barrier:
 * @global: a #SagarmathaGlobal
 * @barrier: a pointer barrier
 *
 * Destroys the @barrier created by sagarmatha_global_create_pointer_barrier().
 */
void
sagarmatha_global_destroy_pointer_barrier (SagarmathaGlobal *global, guint32 barrier)
{
#if HAVE_XFIXESCREATEPOINTERBARRIER
  g_return_if_fail (barrier > 0);

  XFixesDestroyPointerBarrier (global->xdisplay, (PointerBarrier)barrier);
#endif
}


/**
 * sagarmatha_global_add_extension_importer:
 * @target_object_script: JavaScript code evaluating to a target object
 * @target_property: Name of property to use for importer
 * @directory: Source directory:
 * @error: A #GError
 *
 * This function sets a property named @target_property on the object
 * resulting from the evaluation of @target_object_script code, which
 * acts as a GJS importer for directory @directory.
 *
 * Returns: %TRUE on success
 */
gboolean
sagarmatha_global_add_extension_importer (SagarmathaGlobal *global,
                                     const char  *target_object_script,
                                     const char  *target_property,
                                     const char  *directory,
                                     GError     **error)
{
  jsval target_object;
  JSContext *context = gjs_context_get_native_context (global->js_context);
  char *search_path[2] = { 0, 0 };

  JS_BeginRequest (context);

  // This is a bit of a hack; ideally we'd be able to pass our target
  // object directly into this function, but introspection doesn't
  // support that at the moment.  Instead evaluate a string to get it.
  if (!JS_EvaluateScript(context,
                         JS_GetGlobalObject(context),
                         target_object_script,
                         strlen (target_object_script),
                         "<target_object_script>",
                         0,
                         &target_object))
    {
      char *message;
      gjs_log_exception(context,
                        &message);
      g_set_error(error,
                  G_IO_ERROR,
                  G_IO_ERROR_FAILED,
                  "%s", message ? message : "(unknown)");
      g_free(message);
      goto out_error;
    }

  if (!JSVAL_IS_OBJECT (target_object))
    {
      g_error ("sagarmatha_global_add_extension_importer: invalid target object");
      goto out_error;
    }

  search_path[0] = (char*)directory;
  gjs_define_importer (context, JSVAL_TO_OBJECT (target_object), target_property, (const char **)search_path, FALSE);
  JS_EndRequest (context);
  return TRUE;
 out_error:
  JS_EndRequest (context);
  return FALSE;
}

/* Code to close all file descriptors before we exec; copied from gspawn.c in GLib.
 *
 * Authors: Padraig O'Briain, Matthias Clasen, Lennart Poettering
 *
 * http://bugzilla.gnome.org/show_bug.cgi?id=469231
 * http://bugzilla.gnome.org/show_bug.cgi?id=357585
 */

static int
set_cloexec (void *data, gint fd)
{
  if (fd >= GPOINTER_TO_INT (data))
    fcntl (fd, F_SETFD, FD_CLOEXEC);

  return 0;
}

#ifndef HAVE_FDWALK
static int
fdwalk (int (*cb)(void *data, int fd), void *data)
{
  gint open_max;
  gint fd;
  gint res = 0;

#ifdef HAVE_SYS_RESOURCE_H
  struct rlimit rl;
#endif

#ifdef __linux__
  DIR *d;

  if ((d = opendir("/proc/self/fd"))) {
      struct dirent *de;

      while ((de = readdir(d))) {
          glong l;
          gchar *e = NULL;

          if (de->d_name[0] == '.')
              continue;

          errno = 0;
          l = strtol(de->d_name, &e, 10);
          if (errno != 0 || !e || *e)
              continue;

          fd = (gint) l;

          if ((glong) fd != l)
              continue;

          if (fd == dirfd(d))
              continue;

          if ((res = cb (data, fd)) != 0)
              break;
        }

      closedir(d);
      return res;
  }

  /* If /proc is not mounted or not accessible we fall back to the old
   * rlimit trick */

#endif

#ifdef HAVE_SYS_RESOURCE_H
  if (getrlimit(RLIMIT_NOFILE, &rl) == 0 && rl.rlim_max != RLIM_INFINITY)
      open_max = rl.rlim_max;
  else
#endif
      open_max = sysconf (_SC_OPEN_MAX);

  for (fd = 0; fd < open_max; fd++)
      if ((res = cb (data, fd)) != 0)
          break;

  return res;
}
#endif

static void
pre_exec_close_fds(void)
{
  fdwalk (set_cloexec, GINT_TO_POINTER(3));
}

/**
 * sagarmatha_global_reexec_self:
 * @global: A #SagarmathaGlobal
 * 
 * Restart the current process.  Only intended for development purposes. 
 */
void 
sagarmatha_global_reexec_self (SagarmathaGlobal *global)
{
  GPtrArray *arr;
  gsize len;
  char *buf;
  char *buf_p;
  char *buf_end;
  GError *error = NULL;
  
  /* Linux specific (I think, anyways). */
  if (!g_file_get_contents ("/proc/self/cmdline", &buf, &len, &error))
    {
      g_warning ("failed to get /proc/self/cmdline: %s", error->message);
      return;
    }
      
  buf_end = buf+len;
  arr = g_ptr_array_new ();
  /* The cmdline file is NUL-separated */
  for (buf_p = buf; buf_p < buf_end; buf_p = buf_p + strlen (buf_p) + 1)
    g_ptr_array_add (arr, buf_p);
  
  g_ptr_array_add (arr, NULL);

  /* Close all file descriptors other than stdin/stdout/stderr, otherwise
   * they will leak and stay open after the exec. In particular, this is
   * important for file descriptors that represent mapped graphics buffer
   * objects.
   */
  pre_exec_close_fds ();

  meta_display_unmanage_screen (sagarmatha_global_get_display (global),
                                sagarmatha_global_get_screen (global),
                                sagarmatha_global_get_current_time (global));

  execvp (arr->pdata[0], (char**)arr->pdata);
  g_warning ("failed to reexec: %s", g_strerror (errno));
  g_ptr_array_free (arr, TRUE);
}

/**
 * sagarmatha_global_gc:
 * @global: A #SagarmathaGlobal
 *
 * Start a garbage collection process.  For more information, see
 * https://developer.mozilla.org/En/JS_GC
 */
void
sagarmatha_global_gc (SagarmathaGlobal *global)
{
  JSContext *context = gjs_context_get_native_context (global->js_context);

  JS_GC (context);
}

/**
 * sagarmatha_global_maybe_gc:
 * @global: A #SagarmathaGlobal
 *
 * Start a garbage collection process when it would free up enough memory
 * to be worth the amount of time it would take
 * https://developer.mozilla.org/en/SpiderMonkey/JSAPI_Reference/JS_MaybeGC
 */
void
sagarmatha_global_maybe_gc (SagarmathaGlobal *global)
{
  gjs_context_maybe_gc (global->js_context);
}

static void
sagarmatha_global_on_gc (GjsContext   *context,
                    SagarmathaGlobal  *global)
{
  global->last_gc_end_time = g_get_monotonic_time ();
}

/**
 * sagarmatha_global_get_memory_info:
 * @global:
 * @meminfo: (out caller-allocates): Output location for memory information
 *
 * Load process-global data about memory usage.
 */
void
sagarmatha_global_get_memory_info (SagarmathaGlobal        *global,
                              SagarmathaMemoryInfo    *meminfo)
{
  JSContext *context;
  gint64 now;

  memset (meminfo, 0, sizeof (*meminfo));
#ifdef HAVE_MALLINFO
  {
    struct mallinfo info = mallinfo ();
    meminfo->glibc_uordblks = info.uordblks;
  }
#endif

  context = gjs_context_get_native_context (global->js_context);

  meminfo->js_bytes = JS_GetGCParameter (JS_GetRuntime (context), JSGC_BYTES);

  meminfo->gjs_boxed = (unsigned int) gjs_counter_boxed.value;
  meminfo->gjs_gobject = (unsigned int) gjs_counter_object.value;
  meminfo->gjs_function = (unsigned int) gjs_counter_function.value;
  meminfo->gjs_closure = (unsigned int) gjs_counter_closure.value;

  now = g_get_monotonic_time ();

  meminfo->last_gc_seconds_ago = (now - global->last_gc_end_time) / G_TIME_SPAN_SECOND;
}


/**
 * sagarmatha_global_notify_error:
 * @global: a #SagarmathaGlobal
 * @msg: Error message
 * @details: Error details
 *
 * Show a system error notification.  Use this function
 * when a user-initiated action results in a non-fatal problem
 * from causes that may not be under system control.  For
 * example, an application crash.
 */
void
sagarmatha_global_notify_error (SagarmathaGlobal  *global,
                           const char   *msg,
                           const char   *details)
{
  g_signal_emit_by_name (global, "notify-error", msg, details);
}

static void
grab_notify (GtkWidget *widget, gboolean was_grabbed, gpointer user_data)
{
  SagarmathaGlobal *global = SAGARMATHA_GLOBAL (user_data);
  
  global->gtk_grab_active = !was_grabbed;

  /* Update for the new setting of gtk_grab_active */
  sagarmatha_global_set_stage_input_mode (global, global->input_mode);
}

/**
 * sagarmatha_global_init_xdnd:
 * @global: the #SagarmathaGlobal
 *
 * Enables tracking of Xdnd events
 */
void sagarmatha_global_init_xdnd (SagarmathaGlobal *global)
{
  Window output_window = meta_get_overlay_window (global->meta_screen);
  long xdnd_version = 5;

  XChangeProperty (global->xdisplay, global->stage_xwindow,
                   gdk_x11_get_xatom_by_name ("XdndAware"), XA_ATOM,
                   32, PropModeReplace, (const unsigned char *)&xdnd_version, 1);

  XChangeProperty (global->xdisplay, output_window,
                   gdk_x11_get_xatom_by_name ("XdndProxy"), XA_WINDOW,
                   32, PropModeReplace, (const unsigned char *)&global->stage_xwindow, 1);

  /*
   * XdndProxy is additionally set on the proxy window as verification that the
   * XdndProxy property on the target window isn't a left-over
   */
  XChangeProperty (global->xdisplay, global->stage_xwindow,
                   gdk_x11_get_xatom_by_name ("XdndProxy"), XA_WINDOW,
                   32, PropModeReplace, (const unsigned char *)&global->stage_xwindow, 1);
}

/**
 * sagarmatha_global_get_pointer:
 * @global: the #SagarmathaGlobal
 * @x: (out): the X coordinate of the pointer, in global coordinates
 * @y: (out): the Y coordinate of the pointer, in global coordinates
 * @mods: (out): the current set of modifier keys that are pressed down
 *
 * Gets the pointer coordinates and current modifier key state.
 * This is a wrapper around gdk_display_get_pointer() that strips
 * out any un-declared modifier flags, to make gjs happy; see
 * https://bugzilla.gnome.org/show_bug.cgi?id=597292.
 */
void
sagarmatha_global_get_pointer (SagarmathaGlobal         *global,
                          int                 *x,
                          int                 *y,
                          ClutterModifierType *mods)
{
  GdkDeviceManager *gmanager;
  GdkDevice *gdevice;
  GdkScreen *gscreen;
  GdkModifierType raw_mods;
  
  gmanager = gdk_display_get_device_manager (global->gdk_display);
  gdevice = gdk_device_manager_get_client_pointer (gmanager);
  gdk_device_get_position (gdevice, &gscreen, x, y);
  gdk_device_get_state (gdevice,
                        gdk_screen_get_root_window (gscreen),
                        NULL,
                        &raw_mods);
  *mods = raw_mods & GDK_MODIFIER_MASK;
}

/**
 * sagarmatha_global_set_pointer:
 * @global: the #SagarmathaGlobal
 * @x: (in): the X coordinate of the pointer, in global coordinates
 * @y: (in): the Y coordinate of the pointer, in global coordinates
 *
 * Sets the pointer coordinates.
 * This is a wrapper around gdk_device_warp().
 */
void
sagarmatha_global_set_pointer (SagarmathaGlobal         *global,
                          int                 x,
                          int                 y)
{
  GdkDeviceManager *gmanager;
  GdkDevice *gdevice;
  GdkScreen *gscreen;
  int x2, y2;
  
  gmanager = gdk_display_get_device_manager (global->gdk_display);
  gdevice = gdk_device_manager_get_client_pointer (gmanager);
  gdk_device_get_position (gdevice, &gscreen, &x2, &y2);
  gdk_device_warp (gdevice, gscreen, x, y);
}

/**
 * sagarmatha_global_sync_pointer:
 * @global: the #SagarmathaGlobal
 *
 * Ensures that clutter is aware of the current pointer position,
 * causing enter and leave events to be emitted if the pointer moved
 * behind our back (ie, during a pointer grab).
 */
void
sagarmatha_global_sync_pointer (SagarmathaGlobal *global)
{
  int x, y;
  GdkDeviceManager *gmanager;
  GdkDevice *gdevice;
  GdkScreen *gscreen;
  GdkModifierType mods;
  ClutterMotionEvent event;

  gmanager = gdk_display_get_device_manager (global->gdk_display);
  gdevice = gdk_device_manager_get_client_pointer (gmanager);
  gdk_device_get_position (gdevice, &gscreen, &x, &y);
  gdk_device_get_state (gdevice,
                        gdk_screen_get_root_window (gscreen),
                        NULL,
                        &mods);

  event.type = CLUTTER_MOTION;
  event.time = sagarmatha_global_get_current_time (global);
  event.flags = 0;
  /* This is wrong: we should be setting event.stage to NULL if the
   * pointer is not inside the bounds of the stage given the current
   * stage_input_mode. For our current purposes however, this works.
   */
  event.stage = global->stage;
  event.x = x;
  event.y = y;
  event.modifier_state = mods;
  event.axes = NULL;
  event.device = clutter_device_manager_get_core_device (clutter_device_manager_get_default (),
                                                         CLUTTER_POINTER_DEVICE);

  /* Leaving event.source NULL will force clutter to look it up, which
   * will generate enter/leave events as a side effect, if they are
   * needed. We need a better way to do this though... see
   * http://bugzilla.clutter-project.org/show_bug.cgi?id=2615.
   */
  event.source = NULL;

  clutter_event_put ((ClutterEvent *)&event);
}

/**
 * sagarmatha_global_get_settings:
 * @global: A #SagarmathaGlobal
 *
 * Get the global GSettings instance.
 *
 * Return value: (transfer none): The GSettings object
 */
GSettings *
sagarmatha_global_get_settings (SagarmathaGlobal *global)
{
  return global->settings;
}

/**
 * sagarmatha_global_get_current_time:
 * @global: A #SagarmathaGlobal
 *
 * Returns: the current X server time from the current Clutter, Gdk, or X
 * event. If called from outside an event handler, this may return
 * %Clutter.CURRENT_TIME (aka 0), or it may return a slightly
 * out-of-date timestamp.
 */
guint32
sagarmatha_global_get_current_time (SagarmathaGlobal *global)
{
  guint32 time;
  const ClutterEvent *clutter_event;

  /* In case we have a xdnd timestamp use it */
  if (global->xdnd_timestamp != 0)
    return global->xdnd_timestamp;

  /* meta_display_get_current_time() will return the correct time
     when handling an X or Gdk event, but will return CurrentTime
     from some Clutter event callbacks.

     clutter_get_current_event_time() will return the correct time
     from a Clutter event callback, but may return an out-of-date
     timestamp if called at other times.

     So we try meta_display_get_current_time() first, since we
     can recognize a "wrong" answer from that, and then fall back
     to clutter_get_current_event_time().
   */

  time = meta_display_get_current_time (global->meta_display);
  if (time != CLUTTER_CURRENT_TIME)
      return time;
  /*
   * We don't use clutter_get_current_event_time as it can give us a
   * too old timestamp if there is no current event.
   */
  clutter_event = clutter_get_current_event ();

  if (clutter_event != NULL)
    return clutter_event_get_time (clutter_event);
  else
    return CLUTTER_CURRENT_TIME;
}

/**
 * sagarmatha_global_get_pid:
 *
 * Returns: the pid of the sagarmatha process.
 */
pid_t
sagarmatha_global_get_pid ()
{
  return getpid();
}

/**
 * sagarmatha_global_get_md5_for_string:
 *
 * Returns: the MD5 sum for the given string
 */

gchar *
sagarmatha_global_get_md5_for_string (const gchar *string)
{
    return g_compute_checksum_for_string (G_CHECKSUM_MD5, string, -1);
}

/**
 * sagarmatha_global_create_app_launch_context:
 * @global: A #SagarmathaGlobal
 *
 * Create a #GAppLaunchContext set up with the correct timestamp, and
 * targeted to activate on the current workspace.
 *
 * Return value: (transfer full): A new #GAppLaunchContext
 */
GAppLaunchContext *
sagarmatha_global_create_app_launch_context (SagarmathaGlobal *global)
{
  GdkAppLaunchContext *context;

  context = gdk_display_get_app_launch_context (global->gdk_display);
  
  gdk_app_launch_context_set_timestamp (context, sagarmatha_global_get_current_time (global));

  // Make sure that the app is opened on the current workspace even if
  // the user switches before it starts
  gdk_app_launch_context_set_desktop (context, meta_screen_get_active_workspace_index (global->meta_screen));

  return (GAppLaunchContext *)context;
}

typedef struct
{
  SagarmathaLeisureFunction func;
  gpointer user_data;
  GDestroyNotify notify;
} LeisureClosure;

static gboolean
run_leisure_functions (gpointer data)
{
  SagarmathaGlobal *global = data;
  GSList *closures;
  GSList *iter;

  global->leisure_function_id = 0;

  /* We started more work since we scheduled the idle */
  if (global->work_count > 0)
    return FALSE;

  /* Previously we called gjs_maybe_gc().  However, it simply doesn't
   * trigger often enough.  Garbage collection is very fast here, so
   * let's just aggressively GC.  This will help avoid both heap
   * fragmentation, and the GC kicking in when we don't want it to.
   */
  gjs_context_gc (global->js_context);

  /* No leisure closures, so we are done */
  if (global->leisure_closures == NULL)
    return FALSE;

  closures = global->leisure_closures;
  global->leisure_closures = NULL;

  for (iter = closures; iter; iter = iter->next)
    {
      LeisureClosure *closure = closures->data;
      closure->func (closure->user_data);

      if (closure->notify)
        closure->notify (closure->user_data);

      g_slice_free (LeisureClosure, closure);
    }

  g_slist_free (closures);

  return FALSE;
}

static void
schedule_leisure_functions (SagarmathaGlobal *global)
{
  /* This is called when we think we are ready to run leisure functions
   * by our own accounting. We try to handle other types of business
   * (like ClutterAnimation) by adding a low priority idle function.
   *
   * This won't work properly if the mainloop goes idle waiting for
   * the vertical blanking interval or waiting for work being done
   * in another thread.
   */
  if (!global->leisure_function_id)
    global->leisure_function_id = g_idle_add_full (G_PRIORITY_LOW,
                                                   run_leisure_functions,
                                                   global, NULL);
}

/**
 * sagarmatha_global_begin_work:
 * @global: the #SagarmathaGlobal
 *
 * Marks that we are currently doing work. This is used to to track
 * whether we are busy for the purposes of sagarmatha_global_run_at_leisure().
 * A count is kept and sagarmatha_global_end_work() must be called exactly
 * as many times as sagarmatha_global_begin_work().
 */
void
sagarmatha_global_begin_work (SagarmathaGlobal *global)
{
  global->work_count++;
}

/**
 * sagarmatha_global_end_work:
 * @global: the #SagarmathaGlobal
 *
 * Marks the end of work that we started with sagarmatha_global_begin_work().
 * If no other work is ongoing and functions have been added with
 * sagarmatha_global_run_at_leisure(), they will be run at the next
 * opportunity.
 */
void
sagarmatha_global_end_work (SagarmathaGlobal *global)
{
  g_return_if_fail (global->work_count > 0);

  global->work_count--;
  if (global->work_count == 0)
    schedule_leisure_functions (global);

}

/**
 * sagarmatha_global_run_at_leisure:
 * @global: the #SagarmathaGlobal
 * @func: function to call at leisure
 * @user_data: data to pass to @func
 * @notify: function to call to free @user_data
 *
 * Schedules a function to be called the next time Sagarmatha is idle.
 * Idle means here no animations, no redrawing, and no ongoing background
 * work. Since there is currently no way to hook into the Clutter master
 * clock and know when is running, the implementation here is somewhat
 * approximation. Animations done through Sagarmatha's Tweener module will
 * be handled properly, but other animations may be detected as terminating
 * early if they can be drawn fast enough so that the event loop goes idle
 * between frames.
 *
 * The intent of this function is for performance measurement runs
 * where a number of actions should be run serially and each action is
 * timed individually. Using this function for other purposes will
 * interfere with the ability to use it for performance measurement so
 * should be avoided.
 */
void
sagarmatha_global_run_at_leisure (SagarmathaGlobal         *global,
                             SagarmathaLeisureFunction func,
                             gpointer             user_data,
                             GDestroyNotify       notify)
{
  LeisureClosure *closure = g_slice_new (LeisureClosure);
  closure->func = func;
  closure->user_data = user_data;
  closure->notify = notify;

  global->leisure_closures = g_slist_append (global->leisure_closures,
                                             closure);

  if (global->work_count == 0)
    schedule_leisure_functions (global);
}

/**
 * sagarmatha_global_play_theme_sound:
 * @global: the #SagarmathaGlobal
 * @id: an id, used to cancel later (0 if not needed)
 * @name: the sound name
 *
 * Plays a simple sound picked according to Freedesktop sound theme.
 * Really just a workaround for libcanberra not being introspected.
 */
void
sagarmatha_global_play_theme_sound (SagarmathaGlobal *global,
                               guint        id,
                               const char  *name)
{
  ca_context_play (global->sound_context, id, CA_PROP_EVENT_ID, name, NULL);
}

/**
 * sagarmatha_global_play_sound_file:
 * @global: the #SagarmathaGlobal
 * @id: an id, used to cancel later (0 if not needed)
 * @filename: the filename of the sound
 *
 * Plays a simple sound.
 * Really just a workaround for libcanberra not being introspected.
 */
void
sagarmatha_global_play_sound_file (SagarmathaGlobal *global,
                               guint        id,
                               const char  *filename)
{
  ca_context_play (global->sound_context, id, CA_PROP_MEDIA_FILENAME, filename, NULL);
}

/**
 * sagarmatha_global_cancel_sound:
 * @global: the #SagarmathaGlobal
 * @id: the id previously passed to sagarmatha_global_play_theme_sound() or sagarmatha_global_play_sound_file()
 *
 * Cancels a sound being played.
 */
void
sagarmatha_global_cancel_sound (SagarmathaGlobal *global,
                                 guint id)
{
  ca_context_cancel (global->sound_context, id);
}

/*
 * Process Xdnd events
 *
 * We pass the position and leave events to JS via a signal
 * where the actual drag & drop handling happens.
 *
 * http://www.freedesktop.org/wiki/Specifications/XDND
 */
gboolean _sagarmatha_global_check_xdnd_event (SagarmathaGlobal  *global,
                                         XEvent       *xev)
{
  Window output_window = meta_get_overlay_window (global->meta_screen);

  if (xev->xany.window != output_window && xev->xany.window != global->stage_xwindow)
    return FALSE;

  if (xev->xany.type == ClientMessage && xev->xclient.message_type == gdk_x11_get_xatom_by_name ("XdndPosition"))
    {
      XEvent xevent;
      Window src = xev->xclient.data.l[0];

      memset (&xevent, 0, sizeof(xevent));
      xevent.xany.type = ClientMessage;
      xevent.xany.display = global->xdisplay;
      xevent.xclient.window = src;
      xevent.xclient.message_type = gdk_x11_get_xatom_by_name ("XdndStatus");
      xevent.xclient.format = 32;
      xevent.xclient.data.l[0] = output_window;
      /* flags: bit 0: will we accept the drop? bit 1: do we want more position messages */
      xevent.xclient.data.l[1] = 2;
      xevent.xclient.data.l[4] = None;

      XSendEvent (global->xdisplay, src, False, 0, &xevent);

      /* Store the timestamp of the xdnd position event */
      global->xdnd_timestamp = xev->xclient.data.l[3];
      g_signal_emit_by_name (G_OBJECT (global), "xdnd-position-changed",
                            (int)(xev->xclient.data.l[2] >> 16), (int)(xev->xclient.data.l[2] & 0xFFFF));
      global->xdnd_timestamp = 0;

      return TRUE;
    }
   else if (xev->xany.type == ClientMessage && xev->xclient.message_type == gdk_x11_get_xatom_by_name ("XdndLeave"))
    {
      g_signal_emit_by_name (G_OBJECT (global), "xdnd-leave");

      return TRUE;
    }
   else if (xev->xany.type == ClientMessage && xev->xclient.message_type == gdk_x11_get_xatom_by_name ("XdndEnter"))
    {
      g_signal_emit_by_name (G_OBJECT (global), "xdnd-enter");

      return TRUE;
    }

    return FALSE;
}
