/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __CINNAMON_EMBEDDED_WINDOW_H__
#define __CINNAMON_EMBEDDED_WINDOW_H__

#include <gtk/gtk.h>
#include <clutter/clutter.h>

#define CINNAMON_TYPE_EMBEDDED_WINDOW                 (sagarmatha_embedded_window_get_type ())
#define CINNAMON_EMBEDDED_WINDOW(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), CINNAMON_TYPE_EMBEDDED_WINDOW, SagarmathaEmbeddedWindow))
#define CINNAMON_EMBEDDED_WINDOW_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), CINNAMON_TYPE_EMBEDDED_WINDOW, SagarmathaEmbeddedWindowClass))
#define CINNAMON_IS_EMBEDDED_WINDOW(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CINNAMON_TYPE_EMBEDDED_WINDOW))
#define CINNAMON_IS_EMBEDDED_WINDOW_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), CINNAMON_TYPE_EMBEDDED_WINDOW))
#define CINNAMON_EMBEDDED_WINDOW_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), CINNAMON_TYPE_EMBEDDED_WINDOW, SagarmathaEmbeddedWindowClass))

typedef struct _SagarmathaEmbeddedWindow        SagarmathaEmbeddedWindow;
typedef struct _SagarmathaEmbeddedWindowClass   SagarmathaEmbeddedWindowClass;

typedef struct _SagarmathaEmbeddedWindowPrivate SagarmathaEmbeddedWindowPrivate;

struct _SagarmathaEmbeddedWindow
{
  GtkWindow parent;

  SagarmathaEmbeddedWindowPrivate *priv;
};

struct _SagarmathaEmbeddedWindowClass
{
  GtkWindowClass parent_class;
};

GType sagarmatha_embedded_window_get_type (void) G_GNUC_CONST;
GtkWidget *sagarmatha_embedded_window_new (ClutterStage *stage);

#endif /* __CINNAMON_EMBEDDED_WINDOW_H__ */
