/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#include "config.h"

#include <string.h>

#include <glib/gi18n-lib.h>

#include <meta/display.h>

#include "sagarmatha-app-private.h"
#include "sagarmatha-enum-types.h"
#include "sagarmatha-global.h"
#include "sagarmatha-util.h"
#include "sagarmatha-app-system-private.h"
#include "sagarmatha-window-tracker-private.h"
#include "st.h"

typedef enum {
  MATCH_NONE,
  MATCH_SUBSTRING, /* Not prefix, substring */
  MATCH_PREFIX, /* Strict prefix */
} SagarmathaAppSearchMatch;

/* This is mainly a memory usage optimization - the user is going to
 * be running far fewer of the applications at one time than they have
 * installed.  But it also just helps keep the code more logically
 * separated.
 */
typedef struct {
  guint refcount;

  /* Last time the user interacted with any of this application's windows */
  guint32 last_user_time;

  /* Signal connection to dirty window sort list on workspace changes */
  guint workspace_switch_id;

  GSList *windows;

  /* Whether or not we need to resort the windows; this is done on demand */
  gboolean window_sort_stale : 1;
} SagarmathaAppRunningState;

/**
 * SECTION:sagarmatha-app
 * @short_description: Object representing an application
 *
 * This object wraps a #GMenuTreeEntry, providing methods and signals
 * primarily useful for running applications.
 */
struct _SagarmathaApp
{
  GObject parent;

  int started_on_workspace;

  SagarmathaAppState state;

  GMenuTreeEntry *entry; /* If NULL, this app is backed by one or more
                          * MetaWindow.  For purposes of app title
                          * etc., we use the first window added,
                          * because it's most likely to be what we
                          * want (e.g. it will be of TYPE_NORMAL from
                          * the way sagarmatha-window-tracker.c works).
                          */

  SagarmathaAppRunningState *running_state;

  char *window_id_string;

  char *casefolded_name;
  char *name_collation_key;
  char *casefolded_description;
  char *casefolded_exec;
};

G_DEFINE_TYPE (SagarmathaApp, sagarmatha_app, G_TYPE_OBJECT);

enum {
  PROP_0,
  PROP_STATE
};

enum {
  WINDOWS_CHANGED,
  LAST_SIGNAL
};

static guint sagarmatha_app_signals[LAST_SIGNAL] = { 0 };

static void create_running_state (SagarmathaApp *app);
static void unref_running_state (SagarmathaAppRunningState *state);

static void
sagarmatha_app_get_property (GObject    *gobject,
                        guint       prop_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
  SagarmathaApp *app = SAGARMATHA_APP (gobject);

  switch (prop_id)
    {
    case PROP_STATE:
      g_value_set_enum (value, app->state);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

const char *
sagarmatha_app_get_id (SagarmathaApp *app)
{
  if (app->entry)
    return gmenu_tree_entry_get_desktop_file_id (app->entry);
  return app->window_id_string;
}

static MetaWindow *
window_backed_app_get_window (SagarmathaApp     *app)
{
  g_assert (app->entry == NULL);
  g_assert (app->running_state);
  g_assert (app->running_state->windows);
  return app->running_state->windows->data;
}

static ClutterActor *
window_backed_app_get_icon (SagarmathaApp *app,
                            int       size)
{
  MetaWindow *window;
  ClutterActor *actor;

  /* During a state transition from running to not-running for
   * window-backend apps, it's possible we get a request for the icon.
   * Avoid asserting here and just return an empty image.
   */
  if (app->running_state == NULL)
    {
      actor = clutter_texture_new ();
      g_object_set (actor, "opacity", 0, "width", (float) size, "height", (float) size, NULL);
      return actor;
    }

  window = window_backed_app_get_window (app);
  actor = st_texture_cache_bind_pixbuf_property (st_texture_cache_get_default (),
                                                               G_OBJECT (window),
                                                               "icon");
  g_object_set (actor, "width", (float) size, "height", (float) size, NULL);
  return actor;
}

/**
 * sagarmatha_app_create_icon_texture:
 *
 * Look up the icon for this application, and create a #ClutterTexture
 * for it at the given size.
 *
 * Return value: (transfer none): A floating #ClutterActor
 */
ClutterActor *
sagarmatha_app_create_icon_texture (SagarmathaApp   *app,
                               int         size)
{
  GIcon *icon;
  ClutterActor *ret;

  ret = NULL;

  if (app->entry == NULL)
    return window_backed_app_get_icon (app, size);

  icon = g_app_info_get_icon (G_APP_INFO (gmenu_tree_entry_get_app_info (app->entry)));
  if (icon != NULL)
    ret = st_texture_cache_load_gicon (st_texture_cache_get_default (), NULL, icon, size);

  if (ret == NULL)
    {
      icon = g_themed_icon_new ("application-x-executable");
      ret = st_texture_cache_load_gicon (st_texture_cache_get_default (), NULL, icon, size);
      g_object_unref (icon);
    }

  return ret;
}

typedef struct {
  SagarmathaApp *app;
  int size;
} CreateFadedIconData;

static CoglHandle
sagarmatha_app_create_faded_icon_cpu (StTextureCache *cache,
                                 const char     *key,
                                 void           *datap,
                                 GError        **error)
{
  CreateFadedIconData *data = datap;
  SagarmathaApp *app;
  GdkPixbuf *pixbuf;
  int size;
  CoglHandle texture;
  gint width, height, rowstride;
  guint8 n_channels;
  gboolean have_alpha;
  gint fade_start;
  gint fade_range;
  guint i, j;
  guint pixbuf_byte_size;
  guint8 *orig_pixels;
  guint8 *pixels;
  GIcon *icon;
  GtkIconInfo *info;

  app = data->app;
  size = data->size;

  info = NULL;

  icon = g_app_info_get_icon (G_APP_INFO (gmenu_tree_entry_get_app_info (app->entry)));
  if (icon != NULL)
    {
      info = gtk_icon_theme_lookup_by_gicon (gtk_icon_theme_get_default (),
                                             icon, size,
                                             GTK_ICON_LOOKUP_FORCE_SIZE);
    }

  if (info == NULL)
    {
      icon = g_themed_icon_new ("application-x-executable");
      info = gtk_icon_theme_lookup_by_gicon (gtk_icon_theme_get_default (),
                                             icon, size,
                                             GTK_ICON_LOOKUP_FORCE_SIZE);
      g_object_unref (icon);
    }

  if (info == NULL)
    return COGL_INVALID_HANDLE;

  pixbuf = gtk_icon_info_load_icon (info, NULL);
  gtk_icon_info_free (info);

  if (pixbuf == NULL)
    return COGL_INVALID_HANDLE;

  width = gdk_pixbuf_get_width (pixbuf);
  height = gdk_pixbuf_get_height (pixbuf);
  rowstride = gdk_pixbuf_get_rowstride (pixbuf);
  n_channels = gdk_pixbuf_get_n_channels (pixbuf);
  orig_pixels = gdk_pixbuf_get_pixels (pixbuf);
  have_alpha = gdk_pixbuf_get_has_alpha (pixbuf);

  pixbuf_byte_size = (height - 1) * rowstride +
    + width * ((n_channels * gdk_pixbuf_get_bits_per_sample (pixbuf) + 7) / 8);

  pixels = g_malloc0 (rowstride * height);
  memcpy (pixels, orig_pixels, pixbuf_byte_size);

  fade_start = width / 2;
  fade_range = width - fade_start;
  for (i = fade_start; i < width; i++)
    {
      for (j = 0; j < height; j++)
        {
          guchar *pixel = &pixels[j * rowstride + i * n_channels];
          float fade = 1.0 - ((float) i - fade_start) / fade_range;
          pixel[0] = 0.5 + pixel[0] * fade;
          pixel[1] = 0.5 + pixel[1] * fade;
          pixel[2] = 0.5 + pixel[2] * fade;
          if (have_alpha)
            pixel[3] = 0.5 + pixel[3] * fade;
        }
    }

  texture = cogl_texture_new_from_data (width,
                                        height,
                                        COGL_TEXTURE_NONE,
                                        have_alpha ? COGL_PIXEL_FORMAT_RGBA_8888 : COGL_PIXEL_FORMAT_RGB_888,
                                        COGL_PIXEL_FORMAT_ANY,
                                        rowstride,
                                        pixels);
  g_free (pixels);
  g_object_unref (pixbuf);

  return texture;
}

/**
 * sagarmatha_app_get_faded_icon:
 * @app: A #SagarmathaApp
 * @size: Size in pixels
 *
 * Return an actor with a horizontally faded look.
 *
 * Return value: (transfer none): A floating #ClutterActor, or %NULL if no icon
 */
ClutterActor *
sagarmatha_app_get_faded_icon (SagarmathaApp *app, int size)
{
  CoglHandle texture;
  ClutterActor *result;
  char *cache_key;
  CreateFadedIconData data;

  /* Don't fade for window backed apps for now...easier to reuse the
   * property tracking bits, and this helps us visually distinguish
   * app-tracked from not.
   */
  if (!app->entry)
    return window_backed_app_get_icon (app, size);

  cache_key = g_strdup_printf ("faded-icon:%s,size=%d", sagarmatha_app_get_id (app), size);
  data.app = app;
  data.size = size;
  texture = st_texture_cache_load (st_texture_cache_get_default (),
                                   cache_key,
                                   ST_TEXTURE_CACHE_POLICY_FOREVER,
                                   sagarmatha_app_create_faded_icon_cpu,
                                   &data,
                                   NULL);
  g_free (cache_key);

  if (texture != COGL_INVALID_HANDLE)
    {
      result = clutter_texture_new ();
      clutter_texture_set_cogl_texture (CLUTTER_TEXTURE (result), texture);
    }
  else
    {
      result = clutter_texture_new ();
      g_object_set (result, "opacity", 0, "width", (float) size, "height", (float) size, NULL);

    }
  return result;
}

const char *
sagarmatha_app_get_name (SagarmathaApp *app)
{
  if (app->entry)
    return g_app_info_get_name (G_APP_INFO (gmenu_tree_entry_get_app_info (app->entry)));
  else if (app->running_state == NULL)
    return _("Unknown");
  else
    {
      MetaWindow *window = window_backed_app_get_window (app);
      const char *name;

      name = meta_window_get_wm_class (window);
      if (!name)
        name = _("Unknown");
      return name;
    }
}

const char *
sagarmatha_app_get_description (SagarmathaApp *app)
{
  if (app->entry)
    return g_app_info_get_description (G_APP_INFO (gmenu_tree_entry_get_app_info (app->entry)));
  else
    return NULL;
}

/**
 * sagarmatha_app_is_window_backed:
 *
 * A window backed application is one which represents just an open
 * window, i.e. there's no .desktop file assocation, so we don't know
 * how to launch it again.
 */
gboolean
sagarmatha_app_is_window_backed (SagarmathaApp *app)
{
  return app->entry == NULL;
}

typedef struct {
  MetaWorkspace *workspace;
  GSList **transients;
} CollectTransientsData;

static gboolean
collect_transients_on_workspace (MetaWindow *window,
                                 gpointer    datap)
{
  CollectTransientsData *data = datap;

  if (data->workspace && meta_window_get_workspace (window) != data->workspace)
    return TRUE;

  *data->transients = g_slist_prepend (*data->transients, window);
  return TRUE;
}

/* The basic idea here is that when we're targeting a window,
 * if it has transients we want to pick the most recent one
 * the user interacted with.
 * This function makes raising GEdit with the file chooser
 * open work correctly.
 */
static MetaWindow *
find_most_recent_transient_on_same_workspace (MetaDisplay *display,
                                              MetaWindow  *reference)
{
  GSList *transients, *transients_sorted, *iter;
  MetaWindow *result;
  CollectTransientsData data;

  transients = NULL;
  data.workspace = meta_window_get_workspace (reference);
  data.transients = &transients;

  meta_window_foreach_transient (reference, collect_transients_on_workspace, &data);

  transients_sorted = meta_display_sort_windows_by_stacking (display, transients);
  /* Reverse this so we're top-to-bottom (yes, we should probably change the order
   * returned from the sort_windows_by_stacking function)
   */
  transients_sorted = g_slist_reverse (transients_sorted);
  g_slist_free (transients);
  transients = NULL;

  result = NULL;
  for (iter = transients_sorted; iter; iter = iter->next)
    {
      MetaWindow *window = iter->data;
      MetaWindowType wintype = meta_window_get_window_type (window);

      /* Don't want to focus UTILITY types, like the Gimp toolbars */
      if (wintype == META_WINDOW_NORMAL ||
          wintype == META_WINDOW_DIALOG)
        {
          result = window;
          break;
        }
    }
  g_slist_free (transients_sorted);
  return result;
}

/**
 * sagarmatha_app_activate_window:
 * @app: a #SagarmathaApp
 * @window: (allow-none): Window to be focused
 * @timestamp: Event timestamp
 *
 * Bring all windows for the given app to the foreground,
 * but ensure that @window is on top.  If @window is %NULL,
 * the window with the most recent user time for the app
 * will be used.
 *
 * This function has no effect if @app is not currently running.
 */
void
sagarmatha_app_activate_window (SagarmathaApp     *app,
                           MetaWindow   *window,
                           guint32       timestamp)
{
  GSList *windows;

  if (sagarmatha_app_get_state (app) != SAGARMATHA_APP_STATE_RUNNING)
    return;

  windows = sagarmatha_app_get_windows (app);
  if (window == NULL && windows)
    window = windows->data;

  if (!g_slist_find (windows, window))
    return;
  else
    {
      GSList *iter;
      SagarmathaGlobal *global = sagarmatha_global_get ();
      MetaScreen *screen = sagarmatha_global_get_screen (global);
      MetaDisplay *display = meta_screen_get_display (screen);
      MetaWorkspace *active = meta_screen_get_active_workspace (screen);
      MetaWorkspace *workspace = meta_window_get_workspace (window);
      guint32 last_user_timestamp = meta_display_get_last_user_time (display);
      MetaWindow *most_recent_transient;

      if (meta_display_xserver_time_is_before (display, timestamp, last_user_timestamp))
        {
          meta_window_set_demands_attention (window);
          return;
        }

      /* Now raise all the other windows for the app that are on
       * the same workspace, in reverse order to preserve the stacking.
       */
      for (iter = windows; iter; iter = iter->next)
        {
          MetaWindow *other_window = iter->data;

          if (other_window != window)
            meta_window_raise (other_window);
        }

      /* If we have a transient that the user's interacted with more recently than
       * the window, pick that.
       */
      most_recent_transient = find_most_recent_transient_on_same_workspace (display, window);
      if (most_recent_transient
          && meta_display_xserver_time_is_before (display,
                                                  meta_window_get_user_time (window),
                                                  meta_window_get_user_time (most_recent_transient)))
        window = most_recent_transient;


      if (!sagarmatha_window_tracker_is_window_interesting (window))
        {
          /* We won't get notify::user-time signals for uninteresting windows,
           * which means that an app's last_user_time won't get updated.
           * Update it here instead.
           */
          app->running_state->last_user_time = timestamp;
        }

      if (active != workspace)
        meta_workspace_activate_with_focus (workspace, window, timestamp);
      else
        meta_window_activate (window, timestamp);
    }
}

/**
 * sagarmatha_app_activate:
 * @app: a #SagarmathaApp
 *
 * Like sagarmatha_app_activate_full(), but using the default workspace and
 * event timestamp.
 */
void
sagarmatha_app_activate (SagarmathaApp      *app)
{
  return sagarmatha_app_activate_full (app, -1, 0);
}

/**
 * sagarmatha_app_activate_full:
 * @app: a #SagarmathaApp
 * @workspace: launch on this workspace, or -1 for default. Ignored if
 *   activating an existing window
 * @timestamp: Event timestamp
 *
 * Perform an appropriate default action for operating on this application,
 * dependent on its current state.  For example, if the application is not
 * currently running, launch it.  If it is running, activate the most
 * recently used NORMAL window (or if that window has a transient, the most
 * recently used transient for that window).
 */
void
sagarmatha_app_activate_full (SagarmathaApp      *app,
                         int            workspace,
                         guint32        timestamp)
{
  SagarmathaGlobal *global;

  global = sagarmatha_global_get ();

  if (timestamp == 0)
    timestamp = sagarmatha_global_get_current_time (global);

  switch (app->state)
    {
      case SAGARMATHA_APP_STATE_STOPPED:
        {
          GError *error = NULL;
          if (!sagarmatha_app_launch (app,
                                 timestamp,
                                 NULL,
                                 workspace,
                                 NULL,
                                 &error))
            {
              char *msg;
              msg = g_strdup_printf (_("Failed to launch '%s'"), sagarmatha_app_get_name (app));
              sagarmatha_global_notify_error (global,
                                         msg,
                                         error->message);
              g_free (msg);
              g_clear_error (&error);
            }
        }
        break;
      case SAGARMATHA_APP_STATE_STARTING:
        break;
      case SAGARMATHA_APP_STATE_RUNNING:
        sagarmatha_app_activate_window (app, NULL, timestamp);
        break;
    }
}

/**
 * sagarmatha_app_open_new_window:
 * @app: a #SagarmathaApp
 * @workspace: open on this workspace, or -1 for default
 *
 * Request that the application create a new window.
 */
void
sagarmatha_app_open_new_window (SagarmathaApp      *app,
                           int            workspace)
{
  g_return_if_fail (app->entry != NULL);

  /* Here we just always launch the application again, even if we know
   * it was already running.  For most applications this
   * should have the effect of creating a new window, whether that's
   * a second process (in the case of Calculator) or IPC to existing
   * instance (Firefox).  There are a few less-sensical cases such
   * as say Pidgin.  Ideally, we have the application express to us
   * that it supports an explicit new-window action.
   */
  sagarmatha_app_launch (app,
                    0,
                    NULL,
                    workspace,
                    NULL,
                    NULL);
}

/**
 * sagarmatha_app_get_state:
 * @app: a #SagarmathaApp
 *
 * Returns: State of the application
 */
SagarmathaAppState
sagarmatha_app_get_state (SagarmathaApp *app)
{
  return app->state;
}

typedef struct {
  SagarmathaApp *app;
  MetaWorkspace *active_workspace;
} CompareWindowsData;

static int
sagarmatha_app_compare_windows (gconstpointer   a,
                           gconstpointer   b,
                           gpointer        datap)
{
  MetaWindow *win_a = (gpointer)a;
  MetaWindow *win_b = (gpointer)b;
  CompareWindowsData *data = datap;
  gboolean ws_a, ws_b;
  gboolean vis_a, vis_b;

  ws_a = meta_window_get_workspace (win_a) == data->active_workspace;
  ws_b = meta_window_get_workspace (win_b) == data->active_workspace;

  if (ws_a && !ws_b)
    return -1;
  else if (!ws_a && ws_b)
    return 1;

  vis_a = meta_window_showing_on_its_workspace (win_a);
  vis_b = meta_window_showing_on_its_workspace (win_b);

  if (vis_a && !vis_b)
    return -1;
  else if (!vis_a && vis_b)
    return 1;

  return meta_window_get_user_time (win_b) - meta_window_get_user_time (win_a);
}

/**
 * sagarmatha_app_get_windows:
 * @app:
 *
 * Get the toplevel, interesting windows which are associated with this
 * application.  The returned list will be sorted first by whether
 * they're on the active workspace, then by whether they're visible,
 * and finally by the time the user last interacted with them.
 *
 * Returns: (transfer none) (element-type MetaWindow): List of windows
 */
GSList *
sagarmatha_app_get_windows (SagarmathaApp *app)
{
  if (app->running_state == NULL)
    return NULL;

  if (app->running_state->window_sort_stale)
    {
      CompareWindowsData data;
      data.app = app;
      data.active_workspace = meta_screen_get_active_workspace (sagarmatha_global_get_screen (sagarmatha_global_get ()));
      app->running_state->windows = g_slist_sort_with_data (app->running_state->windows, sagarmatha_app_compare_windows, &data);
      app->running_state->window_sort_stale = FALSE;
    }

  return app->running_state->windows;
}

guint
sagarmatha_app_get_n_windows (SagarmathaApp *app)
{
  if (app->running_state == NULL)
    return 0;
  return g_slist_length (app->running_state->windows);
}

static gboolean
sagarmatha_app_has_visible_windows (SagarmathaApp   *app)
{
  GSList *iter;

  if (app->running_state == NULL)
    return FALSE;

  for (iter = app->running_state->windows; iter; iter = iter->next)
    {
      MetaWindow *window = iter->data;

      if (meta_window_showing_on_its_workspace (window))
        return TRUE;
    }

  return FALSE;
}

gboolean
sagarmatha_app_is_on_workspace (SagarmathaApp *app,
                           MetaWorkspace   *workspace)
{
  GSList *iter;

  if (sagarmatha_app_get_state (app) == SAGARMATHA_APP_STATE_STARTING)
    {
      if (app->started_on_workspace == -1 ||
          meta_workspace_index (workspace) == app->started_on_workspace)
        return TRUE;
      else
        return FALSE;
    }

  if (app->running_state == NULL)
    return FALSE;

  for (iter = app->running_state->windows; iter; iter = iter->next)
    {
      if (meta_window_get_workspace (iter->data) == workspace)
        return TRUE;
    }

  return FALSE;
}

/**
 * sagarmatha_app_compare:
 * @app:
 * @other: A #SagarmathaApp
 *
 * Compare one #SagarmathaApp instance to another, in the following way:
 *   - Running applications sort before not-running applications.
 *   - If one of them has visible windows and the other does not, the one
 *     with visible windows is first.
 *   - Finally, the application which the user interacted with most recently
 *     compares earlier.
 */
int
sagarmatha_app_compare (SagarmathaApp *app,
                   SagarmathaApp *other)
{
  gboolean vis_app, vis_other;

  if (app->state != other->state)
    {
      if (app->state == SAGARMATHA_APP_STATE_RUNNING)
        return -1;
      return 1;
    }

  vis_app = sagarmatha_app_has_visible_windows (app);
  vis_other = sagarmatha_app_has_visible_windows (other);

  if (vis_app && !vis_other)
    return -1;
  else if (!vis_app && vis_other)
    return 1;

  if (app->state == SAGARMATHA_APP_STATE_RUNNING)
    {
      if (app->running_state->windows && !other->running_state->windows)
        return -1;
      else if (!app->running_state->windows && other->running_state->windows)
        return 1;
      return other->running_state->last_user_time - app->running_state->last_user_time;
    }

  return 0;
}

SagarmathaApp *
_sagarmatha_app_new_for_window (MetaWindow      *window)
{
  SagarmathaApp *app;

  app = g_object_new (SAGARMATHA_TYPE_APP, NULL);

  app->window_id_string = g_strdup_printf ("window:%d", meta_window_get_stable_sequence (window));

  _sagarmatha_app_add_window (app, window);

  return app;
}

SagarmathaApp *
_sagarmatha_app_new (GMenuTreeEntry *info)
{
  SagarmathaApp *app;

  app = g_object_new (SAGARMATHA_TYPE_APP, NULL);

  _sagarmatha_app_set_entry (app, info);

  return app;
}

void
_sagarmatha_app_set_entry (SagarmathaApp       *app,
                      GMenuTreeEntry *entry)
{
  if (app->entry != NULL)
    gmenu_tree_item_unref (app->entry);
  app->entry = gmenu_tree_item_ref (entry);
  
  if (app->name_collation_key != NULL)
    g_free (app->name_collation_key);
  app->name_collation_key = g_utf8_collate_key (sagarmatha_app_get_name (app), -1);
}

static void
sagarmatha_app_state_transition (SagarmathaApp      *app,
                            SagarmathaAppState  state)
{
  if (app->state == state)
    return;
  g_return_if_fail (!(app->state == SAGARMATHA_APP_STATE_RUNNING &&
                      state == SAGARMATHA_APP_STATE_STARTING));
  app->state = state;

  if (app->state == SAGARMATHA_APP_STATE_STOPPED && app->running_state)
    {
      unref_running_state (app->running_state);
      app->running_state = NULL;
    }

  _sagarmatha_app_system_notify_app_state_changed (sagarmatha_app_system_get_default (), app);

  g_object_notify (G_OBJECT (app), "state");
}

static void
sagarmatha_app_on_unmanaged (MetaWindow      *window,
                        SagarmathaApp *app)
{
  _sagarmatha_app_remove_window (app, window);
}

static void
sagarmatha_app_on_user_time_changed (MetaWindow *window,
                                GParamSpec *pspec,
                                SagarmathaApp   *app)
{
  g_assert (app->running_state != NULL);

  app->running_state->last_user_time = meta_window_get_user_time (window);

  /* Ideally we don't want to emit windows-changed if the sort order
   * isn't actually changing. This check catches most of those.
   */
  if (window != app->running_state->windows->data)
    {
      app->running_state->window_sort_stale = TRUE;
      g_signal_emit (app, sagarmatha_app_signals[WINDOWS_CHANGED], 0);
    }
}

static void
sagarmatha_app_on_ws_switch (MetaScreen         *screen,
                        int                 from,
                        int                 to,
                        MetaMotionDirection direction,
                        gpointer            data)
{
  SagarmathaApp *app = SAGARMATHA_APP (data);

  g_assert (app->running_state != NULL);

  app->running_state->window_sort_stale = TRUE;

  g_signal_emit (app, sagarmatha_app_signals[WINDOWS_CHANGED], 0);
}

void
_sagarmatha_app_add_window (SagarmathaApp        *app,
                       MetaWindow      *window)
{
  guint32 user_time;

  if (app->running_state && g_slist_find (app->running_state->windows, window))
    return;

  g_object_freeze_notify (G_OBJECT (app));

  if (!app->running_state)
      create_running_state (app);

  app->running_state->window_sort_stale = TRUE;
  app->running_state->windows = g_slist_prepend (app->running_state->windows, g_object_ref (window));
  g_signal_connect (window, "unmanaged", G_CALLBACK(sagarmatha_app_on_unmanaged), app);
  g_signal_connect (window, "notify::user-time", G_CALLBACK(sagarmatha_app_on_user_time_changed), app);

  user_time = meta_window_get_user_time (window);
  if (user_time > app->running_state->last_user_time)
    app->running_state->last_user_time = user_time;

  if (app->state != SAGARMATHA_APP_STATE_STARTING)
    sagarmatha_app_state_transition (app, SAGARMATHA_APP_STATE_RUNNING);

  g_object_thaw_notify (G_OBJECT (app));

  g_signal_emit (app, sagarmatha_app_signals[WINDOWS_CHANGED], 0);
}

void
_sagarmatha_app_remove_window (SagarmathaApp   *app,
                          MetaWindow *window)
{
  g_assert (app->running_state != NULL);

  if (!g_slist_find (app->running_state->windows, window))
    return;

  g_signal_handlers_disconnect_by_func (window, G_CALLBACK(sagarmatha_app_on_unmanaged), app);
  g_signal_handlers_disconnect_by_func (window, G_CALLBACK(sagarmatha_app_on_user_time_changed), app);
  g_object_unref (window);
  app->running_state->windows = g_slist_remove (app->running_state->windows, window);

  if (app->running_state->windows == NULL)
    sagarmatha_app_state_transition (app, SAGARMATHA_APP_STATE_STOPPED);

  g_signal_emit (app, sagarmatha_app_signals[WINDOWS_CHANGED], 0);
}

/**
 * sagarmatha_app_get_pids:
 * @app: a #SagarmathaApp
 *
 * Returns: (transfer container) (element-type int): An unordered list of process identifers associated with this application.
 */
GSList *
sagarmatha_app_get_pids (SagarmathaApp *app)
{
  GSList *result;
  GSList *iter;

  result = NULL;
  for (iter = sagarmatha_app_get_windows (app); iter; iter = iter->next)
    {
      MetaWindow *window = iter->data;
      int pid = meta_window_get_pid (window);
      /* Note in the (by far) common case, app will only have one pid, so
       * we'll hit the first element, so don't worry about O(N^2) here.
       */
      if (!g_slist_find (result, GINT_TO_POINTER (pid)))
        result = g_slist_prepend (result, GINT_TO_POINTER (pid));
    }
  return result;
}

void
_sagarmatha_app_handle_startup_sequence (SagarmathaApp          *app,
                                    SnStartupSequence *sequence)
{
  gboolean starting = !sn_startup_sequence_get_completed (sequence);

  /* The Sagarmatha design calls for on application launch, the app title
   * appears at top, and no X window is focused.  So when we get
   * a startup-notification for this app, transition it to STARTING
   * if it's currently stopped, set it as our application focus,
   * but focus the no_focus window.
   */
  if (starting && sagarmatha_app_get_state (app) == SAGARMATHA_APP_STATE_STOPPED)
    {
      MetaScreen *screen = sagarmatha_global_get_screen (sagarmatha_global_get ());
      MetaDisplay *display = meta_screen_get_display (screen);

      sagarmatha_app_state_transition (app, SAGARMATHA_APP_STATE_STARTING);
      meta_display_focus_the_no_focus_window (display, screen,
                                              sn_startup_sequence_get_timestamp (sequence));
      app->started_on_workspace = sn_startup_sequence_get_workspace (sequence);
    }

  if (!starting)
    {
      if (app->running_state && app->running_state->windows)
        sagarmatha_app_state_transition (app, SAGARMATHA_APP_STATE_RUNNING);
      else /* application have > 1 .desktop file */
        sagarmatha_app_state_transition (app, SAGARMATHA_APP_STATE_STOPPED);
    }
}

/**
 * sagarmatha_app_request_quit:
 * @app: A #SagarmathaApp
 *
 * Initiate an asynchronous request to quit this application.
 * The application may interact with the user, and the user
 * might cancel the quit request from the application UI.
 *
 * This operation may not be supported for all applications.
 *
 * Returns: %TRUE if a quit request is supported for this application
 */
gboolean
sagarmatha_app_request_quit (SagarmathaApp   *app)
{
  GSList *iter;

  if (sagarmatha_app_get_state (app) != SAGARMATHA_APP_STATE_RUNNING)
    return FALSE;

  /* TODO - check for an XSMP connection; we could probably use that */

  for (iter = app->running_state->windows; iter; iter = iter->next)
    {
      MetaWindow *win = iter->data;

      if (!sagarmatha_window_tracker_is_window_interesting (win))
        continue;

      meta_window_delete (win, sagarmatha_global_get_current_time (sagarmatha_global_get ()));
    }
  return TRUE;
}

static void
_gather_pid_callback (GDesktopAppInfo   *gapp,
                      GPid               pid,
                      gpointer           data)
{
  SagarmathaApp *app;
  SagarmathaWindowTracker *tracker;

  g_return_if_fail (data != NULL);

  app = SAGARMATHA_APP (data);
  tracker = sagarmatha_window_tracker_get_default ();

  _sagarmatha_window_tracker_add_child_process_app (tracker,
                                               pid,
                                               app);
}

/**
 * sagarmatha_app_launch:
 * @timestamp: Event timestamp, or 0 for current event timestamp
 * @uris: (element-type utf8): List of uris to pass to application
 * @workspace: Start on this workspace, or -1 for default
 * @startup_id: (out): Returned startup notification ID, or %NULL if none
 * @error: A #GError
 */
gboolean
sagarmatha_app_launch (SagarmathaApp     *app,
                  guint         timestamp,
                  GList        *uris,
                  int           workspace,
                  char        **startup_id,
                  GError      **error)
{
  GDesktopAppInfo *gapp;
  GdkAppLaunchContext *context;
  gboolean ret;
  SagarmathaGlobal *global;
  MetaScreen *screen;
  GdkDisplay *gdisplay;

  if (startup_id)
    *startup_id = NULL;

  if (app->entry == NULL)
    {
      MetaWindow *window = window_backed_app_get_window (app);
      /* We can't pass URIs into a window; shouldn't hit this
       * code path.  If we do, fix the caller to disallow it.
       */
      g_return_val_if_fail (uris == NULL, TRUE);

      meta_window_activate (window, timestamp);
      return TRUE;
    }

  global = sagarmatha_global_get ();
  screen = sagarmatha_global_get_screen (global);
  gdisplay = gdk_screen_get_display (sagarmatha_global_get_gdk_screen (global));

  if (timestamp == 0)
    timestamp = sagarmatha_global_get_current_time (global);

  if (workspace < 0)
    workspace = meta_screen_get_active_workspace_index (screen);

  context = gdk_display_get_app_launch_context (gdisplay);
  gdk_app_launch_context_set_timestamp (context, timestamp);
  gdk_app_launch_context_set_desktop (context, workspace);

  gapp = gmenu_tree_entry_get_app_info (app->entry);
  ret = g_desktop_app_info_launch_uris_as_manager (gapp, uris,
                                                   G_APP_LAUNCH_CONTEXT (context),
                                                   G_SPAWN_SEARCH_PATH | G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_STDOUT_TO_DEV_NULL  | G_SPAWN_STDERR_TO_DEV_NULL,
                                                   NULL, NULL,
                                                   _gather_pid_callback, app,
                                                   error);
  g_object_unref (context);

  return ret;
}

/**
 * sagarmatha_app_get_app_info:
 * @app: a #SagarmathaApp
 *
 * Returns: (transfer none): The #GDesktopAppInfo for this app, or %NULL if backed by a window
 */
GDesktopAppInfo *
sagarmatha_app_get_app_info (SagarmathaApp *app)
{
  if (app->entry)
    return gmenu_tree_entry_get_app_info (app->entry);
  return NULL;
}

/**
 * sagarmatha_app_get_tree_entry:
 * @app: a #SagarmathaApp
 *
 * Returns: (transfer none): The #GMenuTreeEntry for this app, or %NULL if backed by a window
 */
GMenuTreeEntry *
sagarmatha_app_get_tree_entry (SagarmathaApp *app)
{
  return app->entry;
}

static void
create_running_state (SagarmathaApp *app)
{
  MetaScreen *screen;

  g_assert (app->running_state == NULL);

  screen = sagarmatha_global_get_screen (sagarmatha_global_get ());
  app->running_state = g_slice_new0 (SagarmathaAppRunningState);
  app->running_state->refcount = 1;
  app->running_state->workspace_switch_id =
    g_signal_connect (screen, "workspace-switched", G_CALLBACK(sagarmatha_app_on_ws_switch), app);
}

static void
unref_running_state (SagarmathaAppRunningState *state)
{
  MetaScreen *screen;

  state->refcount--;
  if (state->refcount > 0)
    return;

  screen = sagarmatha_global_get_screen (sagarmatha_global_get ());

  g_signal_handler_disconnect (screen, state->workspace_switch_id);
  g_slice_free (SagarmathaAppRunningState, state);
}

static char *
trim_exec_line (const char *str)
{
  const char *start, *end, *pos;

  if (str == NULL)
    return NULL;

  end = strchr (str, ' ');
  if (end == NULL)
    end = str + strlen (str);

  start = str;
  while ((pos = strchr (start, '/')) && pos < end)
    start = ++pos;

  return g_strndup (start, end - start);
}

static void
sagarmatha_app_init_search_data (SagarmathaApp *app)
{
  const char *name;
  const char *exec;
  const char *comment;
  char *normalized_exec;
  GDesktopAppInfo *appinfo;

  appinfo = gmenu_tree_entry_get_app_info (app->entry);
  name = g_app_info_get_name (G_APP_INFO (appinfo));
  app->casefolded_name = sagarmatha_util_normalize_and_casefold (name);

  comment = g_app_info_get_description (G_APP_INFO (appinfo));
  app->casefolded_description = sagarmatha_util_normalize_and_casefold (comment);

  exec = g_app_info_get_executable (G_APP_INFO (appinfo));
  normalized_exec = sagarmatha_util_normalize_and_casefold (exec);
  app->casefolded_exec = trim_exec_line (normalized_exec);
  g_free (normalized_exec);
}

/**
 * sagarmatha_app_compare_by_name:
 * @app:
 * @other:
 *
 * Order two applications by name.
 *
 * Returns: -1, 0, or 1; suitable for use as a comparison function for e.g. g_slist_sort()
 */
int
sagarmatha_app_compare_by_name (SagarmathaApp *app, SagarmathaApp *other)
{
  return strcmp (app->name_collation_key, other->name_collation_key);
}

static SagarmathaAppSearchMatch
_sagarmatha_app_match_search_terms (SagarmathaApp  *app,
                               GSList    *terms)
{
  GSList *iter;
  SagarmathaAppSearchMatch match;

  if (G_UNLIKELY (!app->casefolded_name))
    sagarmatha_app_init_search_data (app);

  match = MATCH_NONE;
  for (iter = terms; iter; iter = iter->next)
    {
      SagarmathaAppSearchMatch current_match;
      const char *term = iter->data;
      const char *p;

      current_match = MATCH_NONE;

      p = strstr (app->casefolded_name, term);
      if (p != NULL)
        {
          if (p == app->casefolded_name || *(p - 1) == ' ')
            current_match = MATCH_PREFIX;
          else
            current_match = MATCH_SUBSTRING;
        }

      if (app->casefolded_exec)
        {
          p = strstr (app->casefolded_exec, term);
          if (p != NULL)
            {
              if (p == app->casefolded_exec || *(p - 1) == '-')
                current_match = MATCH_PREFIX;
              else if (current_match < MATCH_PREFIX)
                current_match = MATCH_SUBSTRING;
            }
        }

      if (app->casefolded_description && current_match < MATCH_PREFIX)
        {
          /* Only do substring matches, as prefix matches are not meaningful
           * enough for descriptions
           */
          p = strstr (app->casefolded_description, term);
          if (p != NULL)
            current_match = MATCH_SUBSTRING;
        }

      if (current_match == MATCH_NONE)
        return current_match;

      if (current_match > match)
        match = current_match;
    }
  return match;
}

void
_sagarmatha_app_do_match (SagarmathaApp         *app,
                     GSList           *terms,
                     GSList          **prefix_results,
                     GSList          **substring_results)
{
  SagarmathaAppSearchMatch match;
  GAppInfo *appinfo;

  g_assert (app != NULL);

  /* Skip window-backed apps */ 
  appinfo = (GAppInfo*)sagarmatha_app_get_app_info (app);
  if (appinfo == NULL)
    return;
  /* Skip not-visible apps */ 
  if (!g_app_info_should_show (appinfo))
    return;

  match = _sagarmatha_app_match_search_terms (app, terms);
  switch (match)
    {
      case MATCH_NONE:
        break;
      case MATCH_PREFIX:
        *prefix_results = g_slist_prepend (*prefix_results, app);
        break;
      case MATCH_SUBSTRING:
        *substring_results = g_slist_prepend (*substring_results, app);
        break;
    }
}


static void
sagarmatha_app_init (SagarmathaApp *self)
{
  self->state = SAGARMATHA_APP_STATE_STOPPED;
}

static void
sagarmatha_app_dispose (GObject *object)
{
  SagarmathaApp *app = SAGARMATHA_APP (object);

  if (app->entry)
    {
      gmenu_tree_item_unref (app->entry);
      app->entry = NULL;
    }

  if (app->running_state)
    {
      while (app->running_state->windows)
        _sagarmatha_app_remove_window (app, app->running_state->windows->data);
    }

  G_OBJECT_CLASS(sagarmatha_app_parent_class)->dispose (object);
}

static void
sagarmatha_app_finalize (GObject *object)
{
  SagarmathaApp *app = SAGARMATHA_APP (object);

  g_free (app->window_id_string);

  g_free (app->casefolded_name);
  g_free (app->name_collation_key);
  g_free (app->casefolded_description);
  g_free (app->casefolded_exec);

  G_OBJECT_CLASS(sagarmatha_app_parent_class)->finalize (object);
}

static void
sagarmatha_app_class_init(SagarmathaAppClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->get_property = sagarmatha_app_get_property;
  gobject_class->dispose = sagarmatha_app_dispose;
  gobject_class->finalize = sagarmatha_app_finalize;

  sagarmatha_app_signals[WINDOWS_CHANGED] = g_signal_new ("windows-changed",
                                     SAGARMATHA_TYPE_APP,
                                     G_SIGNAL_RUN_LAST,
                                     0,
                                     NULL, NULL,
                                     g_cclosure_marshal_VOID__VOID,
                                     G_TYPE_NONE, 0);

  /**
   * SagarmathaApp:state:
   *
   * The high-level state of the application, effectively whether it's
   * running or not, or transitioning between those states.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_STATE,
                                   g_param_spec_enum ("state",
                                                      "State",
                                                      "Application state",
                                                      SAGARMATHA_TYPE_APP_STATE,
                                                      SAGARMATHA_APP_STATE_STOPPED,
                                                      G_PARAM_READABLE));
}
