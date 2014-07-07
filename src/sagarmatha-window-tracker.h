/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_WINDOW_TRACKER_H__
#define __SAGARMATHA_WINDOW_TRACKER_H__

#include <glib-object.h>
#include <glib.h>
#include <meta/window.h>

#include "sagarmatha-app.h"
#include "sagarmatha-app-system.h"

G_BEGIN_DECLS

typedef struct _SagarmathaWindowTracker SagarmathaWindowTracker;
typedef struct _SagarmathaWindowTrackerClass SagarmathaWindowTrackerClass;
typedef struct _SagarmathaWindowTrackerPrivate SagarmathaWindowTrackerPrivate;

#define SAGARMATHA_TYPE_WINDOW_TRACKER              (sagarmatha_window_tracker_get_type ())
#define SAGARMATHA_WINDOW_TRACKER(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), SAGARMATHA_TYPE_WINDOW_TRACKER, SagarmathaWindowTracker))
#define SAGARMATHA_WINDOW_TRACKER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_WINDOW_TRACKER, SagarmathaWindowTrackerClass))
#define SAGARMATHA_IS_WINDOW_TRACKER(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), SAGARMATHA_TYPE_WINDOW_TRACKER))
#define SAGARMATHA_IS_WINDOW_TRACKER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_WINDOW_TRACKER))
#define SAGARMATHA_WINDOW_TRACKER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_WINDOW_TRACKER, SagarmathaWindowTrackerClass))

struct _SagarmathaWindowTrackerClass
{
  GObjectClass parent_class;
};

GType sagarmatha_window_tracker_get_type (void) G_GNUC_CONST;

SagarmathaWindowTracker* sagarmatha_window_tracker_get_default(void);

SagarmathaApp *sagarmatha_window_tracker_get_window_app (SagarmathaWindowTracker *tracker, MetaWindow *metawin);

SagarmathaApp *sagarmatha_window_tracker_get_app_from_pid (SagarmathaWindowTracker *tracker, int pid);

gboolean sagarmatha_window_tracker_is_window_interesting (MetaWindow *window);

const char *_sagarmatha_window_tracker_get_app_context (SagarmathaWindowTracker *tracker, SagarmathaApp *app);

GSList *sagarmatha_window_tracker_get_startup_sequences (SagarmathaWindowTracker *tracker);

/* Hidden typedef for SnStartupSequence */
typedef struct _SagarmathaStartupSequence SagarmathaStartupSequence;
#define SAGARMATHA_TYPE_STARTUP_SEQUENCE (sagarmatha_startup_sequence_get_type ())
GType sagarmatha_startup_sequence_get_type (void);

const char *sagarmatha_startup_sequence_get_id (SagarmathaStartupSequence *sequence);
SagarmathaApp *sagarmatha_startup_sequence_get_app (SagarmathaStartupSequence *sequence);
const char *sagarmatha_startup_sequence_get_name (SagarmathaStartupSequence *sequence);
gboolean sagarmatha_startup_sequence_get_completed (SagarmathaStartupSequence *sequence);
ClutterActor *sagarmatha_startup_sequence_create_icon (SagarmathaStartupSequence *sequence, guint size);

G_END_DECLS

#endif /* __SAGARMATHA_WINDOW_TRACKER_H__ */
