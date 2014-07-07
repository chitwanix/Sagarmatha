/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_ARROW_H__
#define __SAGARMATHA_ARROW_H__

#include <clutter/clutter.h>
#include <gtk/gtk.h>

#define SAGARMATHA_TYPE_ARROW                 (sagarmatha_arrow_get_type ())
#define SAGARMATHA_ARROW(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), SAGARMATHA_TYPE_ARROW, SagarmathaArrow))
#define SAGARMATHA_ARROW_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_ARROW, SagarmathaArrowClass))
#define SAGARMATHA_IS_ARROW(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SAGARMATHA_TYPE_ARROW))
#define SAGARMATHA_IS_ARROW_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_ARROW))
#define SAGARMATHA_ARROW_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_ARROW, SagarmathaArrowClass))

typedef struct _SagarmathaArrow        SagarmathaArrow;
typedef struct _SagarmathaArrowClass   SagarmathaArrowClass;

typedef struct _SagarmathaArrowPrivate SagarmathaArrowPrivate;

struct _SagarmathaArrow
{
    ClutterCairoTexture parent;

    SagarmathaArrowPrivate *priv;
};

struct _SagarmathaArrowClass
{
    ClutterCairoTextureClass parent_class;
};

GType sagarmatha_arrow_get_type (void) G_GNUC_CONST;

#endif /* __SAGARMATHA_ARROW_H__ */
