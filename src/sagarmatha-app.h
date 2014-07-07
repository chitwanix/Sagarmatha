/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_APP_H__
#define __SAGARMATHA_APP_H__

#include <clutter/clutter.h>
#include <gio/gio.h>
#include <meta/window.h>
#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#include <gmenu-tree.h>

G_BEGIN_DECLS

typedef struct _SagarmathaApp SagarmathaApp;
typedef struct _SagarmathaAppClass SagarmathaAppClass;
typedef struct _SagarmathaAppPrivate SagarmathaAppPrivate;

#define SAGARMATHA_TYPE_APP              (sagarmatha_app_get_type ())
#define SAGARMATHA_APP(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), SAGARMATHA_TYPE_APP, SagarmathaApp))
#define SAGARMATHA_APP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_APP, SagarmathaAppClass))
#define SAGARMATHA_IS_APP(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), SAGARMATHA_TYPE_APP))
#define SAGARMATHA_IS_APP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_APP))
#define SAGARMATHA_APP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_APP, SagarmathaAppClass))

struct _SagarmathaAppClass
{
  GObjectClass parent_class;

};

typedef enum {
  SAGARMATHA_APP_STATE_STOPPED,
  SAGARMATHA_APP_STATE_STARTING,
  SAGARMATHA_APP_STATE_RUNNING
} SagarmathaAppState;

GType sagarmatha_app_get_type (void) G_GNUC_CONST;

const char *sagarmatha_app_get_id (SagarmathaApp *app);
GMenuTreeEntry *sagarmatha_app_get_tree_entry (SagarmathaApp *app);
GDesktopAppInfo *sagarmatha_app_get_app_info (SagarmathaApp *app);

ClutterActor *sagarmatha_app_create_icon_texture (SagarmathaApp *app, int size);
ClutterActor *sagarmatha_app_get_faded_icon (SagarmathaApp *app, int size);
const char *sagarmatha_app_get_name (SagarmathaApp *app);
const char *sagarmatha_app_get_description (SagarmathaApp *app);
gboolean sagarmatha_app_is_window_backed (SagarmathaApp *app);

void sagarmatha_app_activate_window (SagarmathaApp *app, MetaWindow *window, guint32 timestamp);

void sagarmatha_app_activate (SagarmathaApp      *app);

void sagarmatha_app_activate_full (SagarmathaApp      *app,
                              int            workspace,
                              guint32        timestamp);

void sagarmatha_app_open_new_window (SagarmathaApp *app,
                                int       workspace);

SagarmathaAppState sagarmatha_app_get_state (SagarmathaApp *app);

gboolean sagarmatha_app_request_quit (SagarmathaApp *app);

guint sagarmatha_app_get_n_windows (SagarmathaApp *app);

GSList *sagarmatha_app_get_windows (SagarmathaApp *app);

GSList *sagarmatha_app_get_pids (SagarmathaApp *app);

gboolean sagarmatha_app_is_on_workspace (SagarmathaApp *app, MetaWorkspace *workspace);

gboolean sagarmatha_app_launch (SagarmathaApp     *app,
                           guint         timestamp,
                           GList        *uris,
                           int           workspace,
                           char        **startup_id,
                           GError      **error);

int sagarmatha_app_compare_by_name (SagarmathaApp *app, SagarmathaApp *other);

int sagarmatha_app_compare (SagarmathaApp *app, SagarmathaApp *other);

G_END_DECLS

#endif /* __SAGARMATHA_APP_H__ */
