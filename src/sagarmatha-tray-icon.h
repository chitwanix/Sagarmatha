/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_TRAY_ICON_H__
#define __SAGARMATHA_TRAY_ICON_H__

#include "sagarmatha-gtk-embed.h"

#define SAGARMATHA_TYPE_TRAY_ICON                 (sagarmatha_tray_icon_get_type ())
#define SAGARMATHA_TRAY_ICON(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), SAGARMATHA_TYPE_TRAY_ICON, SagarmathaTrayIcon))
#define SAGARMATHA_TRAY_ICON_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_TRAY_ICON, SagarmathaTrayIconClass))
#define SAGARMATHA_IS_TRAY_ICON(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SAGARMATHA_TYPE_TRAY_ICON))
#define SAGARMATHA_IS_TRAY_ICON_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_TRAY_ICON))
#define SAGARMATHA_TRAY_ICON_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_TRAY_ICON, SagarmathaTrayIconClass))

typedef struct _SagarmathaTrayIcon        SagarmathaTrayIcon;
typedef struct _SagarmathaTrayIconClass   SagarmathaTrayIconClass;
typedef struct _SagarmathaTrayIconPrivate SagarmathaTrayIconPrivate;

struct _SagarmathaTrayIcon
{
    SagarmathaGtkEmbed parent;

    SagarmathaTrayIconPrivate *priv;
};

struct _SagarmathaTrayIconClass
{
    SagarmathaGtkEmbedClass parent_class;
};


GType         sagarmatha_tray_icon_get_type (void) G_GNUC_CONST;
ClutterActor *sagarmatha_tray_icon_new      (SagarmathaEmbeddedWindow *window);

void          sagarmatha_tray_icon_click    (SagarmathaTrayIcon       *icon,
                                        ClutterEvent        *event);

#endif /* __SAGARMATHA_TRAY_ICON_H__ */
