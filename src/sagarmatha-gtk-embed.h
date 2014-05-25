/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __CINNAMON_GTK_EMBED_H__
#define __CINNAMON_GTK_EMBED_H__

#include <clutter/x11/clutter-x11.h>

#include "sagarmatha-embedded-window.h"

#define CINNAMON_TYPE_GTK_EMBED                 (sagarmatha_gtk_embed_get_type ())
#define CINNAMON_GTK_EMBED(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), CINNAMON_TYPE_GTK_EMBED, SagarmathaGtkEmbed))
#define CINNAMON_GTK_EMBED_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), CINNAMON_TYPE_GTK_EMBED, SagarmathaGtkEmbedClass))
#define CINNAMON_IS_GTK_EMBED(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CINNAMON_TYPE_GTK_EMBED))
#define CINNAMON_IS_GTK_EMBED_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), CINNAMON_TYPE_GTK_EMBED))
#define CINNAMON_GTK_EMBED_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), CINNAMON_TYPE_GTK_EMBED, SagarmathaGtkEmbedClass))

typedef struct _SagarmathaGtkEmbed        SagarmathaGtkEmbed;
typedef struct _SagarmathaGtkEmbedClass   SagarmathaGtkEmbedClass;
typedef struct _SagarmathaGtkEmbedPrivate SagarmathaGtkEmbedPrivate;

struct _SagarmathaGtkEmbed
{
    ClutterX11TexturePixmap parent;

    SagarmathaGtkEmbedPrivate *priv;
};

struct _SagarmathaGtkEmbedClass
{
    ClutterX11TexturePixmapClass parent_class;
};

GType sagarmatha_gtk_embed_get_type (void) G_GNUC_CONST;
ClutterActor *sagarmatha_gtk_embed_new (SagarmathaEmbeddedWindow *window);

#endif /* __CINNAMON_GTK_EMBED_H__ */
