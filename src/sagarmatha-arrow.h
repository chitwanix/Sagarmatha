/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __CINNAMON_ARROW_H__
#define __CINNAMON_ARROW_H__

#include <clutter/clutter.h>
#include <gtk/gtk.h>

#define CINNAMON_TYPE_ARROW                 (sagarmatha_arrow_get_type ())
#define CINNAMON_ARROW(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), CINNAMON_TYPE_ARROW, SagarmathaArrow))
#define CINNAMON_ARROW_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), CINNAMON_TYPE_ARROW, SagarmathaArrowClass))
#define CINNAMON_IS_ARROW(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CINNAMON_TYPE_ARROW))
#define CINNAMON_IS_ARROW_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), CINNAMON_TYPE_ARROW))
#define CINNAMON_ARROW_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), CINNAMON_TYPE_ARROW, SagarmathaArrowClass))

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

#endif /* __CINNAMON_ARROW_H__ */
