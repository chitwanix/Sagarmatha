/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_GLOBAL_H__
#define __SAGARMATHA_GLOBAL_H__

#include <clutter/clutter.h>
#include <glib-object.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <meta/meta-plugin.h>

G_BEGIN_DECLS

typedef struct _SagarmathaGlobal      SagarmathaGlobal;
typedef struct _SagarmathaGlobalClass SagarmathaGlobalClass;

#define SAGARMATHA_TYPE_GLOBAL              (sagarmatha_global_get_type ())
#define SAGARMATHA_GLOBAL(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), SAGARMATHA_TYPE_GLOBAL, SagarmathaGlobal))
#define SAGARMATHA_GLOBAL_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_GLOBAL, SagarmathaGlobalClass))
#define SAGARMATHA_IS_GLOBAL(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), SAGARMATHA_TYPE_GLOBAL))
#define SAGARMATHA_IS_GLOBAL_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_GLOBAL))
#define SAGARMATHA_GLOBAL_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_GLOBAL, SagarmathaGlobalClass))

struct _SagarmathaGlobalClass
{
  GObjectClass parent_class;
};

GType sagarmatha_global_get_type (void) G_GNUC_CONST;

SagarmathaGlobal   *sagarmatha_global_get                       (void);

ClutterStage  *sagarmatha_global_get_stage                 (SagarmathaGlobal *global);
MetaScreen    *sagarmatha_global_get_screen                (SagarmathaGlobal *global);
GdkScreen     *sagarmatha_global_get_gdk_screen            (SagarmathaGlobal *global);
MetaDisplay   *sagarmatha_global_get_display               (SagarmathaGlobal *global);
GList         *sagarmatha_global_get_window_actors         (SagarmathaGlobal *global);
GSettings     *sagarmatha_global_get_settings              (SagarmathaGlobal *global);
guint32        sagarmatha_global_get_current_time          (SagarmathaGlobal *global);
pid_t          sagarmatha_global_get_pid                      (void);
gchar         *sagarmatha_global_get_md5_for_string        (const gchar *string);

/* Input/event handling */
gboolean sagarmatha_global_begin_modal            (SagarmathaGlobal         *global,
                                              guint32              timestamp);
void     sagarmatha_global_end_modal              (SagarmathaGlobal         *global,
                                              guint32              timestamp);

typedef enum {
  SAGARMATHA_STAGE_INPUT_MODE_NONREACTIVE,
  SAGARMATHA_STAGE_INPUT_MODE_NORMAL,
  SAGARMATHA_STAGE_INPUT_MODE_FOCUSED,
  SAGARMATHA_STAGE_INPUT_MODE_FULLSCREEN
} SagarmathaStageInputMode;

void     sagarmatha_global_set_stage_input_mode   (SagarmathaGlobal         *global,
                                              SagarmathaStageInputMode  mode);
void     sagarmatha_global_set_stage_input_region (SagarmathaGlobal         *global,
                                              GSList              *rectangles);

/* X utilities */
typedef enum {
  SAGARMATHA_CURSOR_DND_IN_DRAG,
  SAGARMATHA_CURSOR_DND_UNSUPPORTED_TARGET,
  SAGARMATHA_CURSOR_DND_MOVE,
  SAGARMATHA_CURSOR_DND_COPY,
  SAGARMATHA_CURSOR_POINTING_HAND
} SagarmathaCursor;

void    sagarmatha_global_set_cursor              (SagarmathaGlobal         *global,
                                              SagarmathaCursor          type);
void    sagarmatha_global_unset_cursor            (SagarmathaGlobal         *global);

guint32 sagarmatha_global_create_pointer_barrier  (SagarmathaGlobal         *global,
                                              int                  x1,
                                              int                  y1,
                                              int                  x2,
                                              int                  y2,
                                              int                  directions);
void    sagarmatha_global_destroy_pointer_barrier (SagarmathaGlobal         *global,
                                              guint32              barrier);

void    sagarmatha_global_get_pointer             (SagarmathaGlobal         *global,
                                              int                 *x,
                                              int                 *y,
                                              ClutterModifierType *mods);
void    sagarmatha_global_set_pointer             (SagarmathaGlobal         *global,
                                              int                 x,
                                              int                 y);


/* JavaScript utilities */
void     sagarmatha_global_gc                   (SagarmathaGlobal *global);
void     sagarmatha_global_maybe_gc             (SagarmathaGlobal *global);

typedef struct {
  guint glibc_uordblks;

  guint js_bytes;

  guint gjs_boxed;
  guint gjs_gobject;
  guint gjs_function;
  guint gjs_closure;

  /* 32 bit to avoid js conversion problems with 64 bit */
  guint  last_gc_seconds_ago;
} SagarmathaMemoryInfo;

void     sagarmatha_global_get_memory_info      (SagarmathaGlobal     *global,
                                            SagarmathaMemoryInfo *meminfo);


/* Run-at-leisure API */
void sagarmatha_global_begin_work     (SagarmathaGlobal          *global);
void sagarmatha_global_end_work       (SagarmathaGlobal          *global);

typedef void (*SagarmathaLeisureFunction) (gpointer data);

void sagarmatha_global_run_at_leisure (SagarmathaGlobal          *global,
                                  SagarmathaLeisureFunction  func,
                                  gpointer              user_data,
                                  GDestroyNotify        notify);


/* Misc utilities / Sagarmatha API */
gboolean sagarmatha_global_add_extension_importer    (SagarmathaGlobal  *global,
                                                 const char   *target_object_script,
                                                 const char   *target_property,
                                                 const char   *directory,
                                                 GError      **error);

void     sagarmatha_global_sync_pointer              (SagarmathaGlobal  *global);

GAppLaunchContext *
         sagarmatha_global_create_app_launch_context (SagarmathaGlobal  *global);

void     sagarmatha_global_play_theme_sound          (SagarmathaGlobal  *global,
                                                 guint         id,
                                                 const char   *name);

void     sagarmatha_global_play_sound_file          (SagarmathaGlobal  *global,
                                                 guint         id,
                                                 const char   *filename);

void     sagarmatha_global_cancel_sound             (SagarmathaGlobal  *global,
                                                 guint         id);

void     sagarmatha_global_notify_error              (SagarmathaGlobal  *global,
                                                 const char   *msg,
                                                 const char   *details);

void     sagarmatha_global_init_xdnd                 (SagarmathaGlobal  *global);

void     sagarmatha_global_reexec_self               (SagarmathaGlobal  *global);

G_END_DECLS

#endif /* __SAGARMATHA_GLOBAL_H__ */
