/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#ifndef __CINNAMON_TRAY_MANAGER_H__
#define __CINNAMON_TRAY_MANAGER_H__

#include <clutter/clutter.h>
#include "st.h"

G_BEGIN_DECLS

#define CINNAMON_TYPE_TRAY_MANAGER			(sagarmatha_tray_manager_get_type ())
#define CINNAMON_TRAY_MANAGER(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), CINNAMON_TYPE_TRAY_MANAGER, SagarmathaTrayManager))
#define CINNAMON_TRAY_MANAGER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), CINNAMON_TYPE_TRAY_MANAGER, SagarmathaTrayManagerClass))
#define CINNAMON_IS_TRAY_MANAGER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), CINNAMON_TYPE_TRAY_MANAGER))
#define CINNAMON_IS_TRAY_MANAGER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), CINNAMON_TYPE_TRAY_MANAGER))
#define CINNAMON_TRAY_MANAGER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), CINNAMON_TYPE_TRAY_MANAGER, SagarmathaTrayManagerClass))
	
typedef struct _SagarmathaTrayManager        SagarmathaTrayManager;
typedef struct _SagarmathaTrayManagerPrivate SagarmathaTrayManagerPrivate;
typedef struct _SagarmathaTrayManagerClass   SagarmathaTrayManagerClass;

struct _SagarmathaTrayManager
{
  GObject parent_instance;

  SagarmathaTrayManagerPrivate *priv;
};

struct _SagarmathaTrayManagerClass
{
  GObjectClass parent_class;

  void (* tray_icon_added)   (SagarmathaTrayManager *manager,
			      ClutterActor     *icon,
			      const char       *lowercase_wm_class);
  void (* tray_icon_removed) (SagarmathaTrayManager *manager,
			      ClutterActor     *icon);

};

GType             sagarmatha_tray_manager_get_type     (void);

SagarmathaTrayManager *sagarmatha_tray_manager_new          (void);
void              sagarmatha_tray_manager_manage_stage (SagarmathaTrayManager *manager,
                                                   ClutterStage     *stage,
                                                   StWidget         *theme_widget);
void              sagarmatha_tray_manager_redisplay (SagarmathaTrayManager *manager);

G_END_DECLS

#endif /* __CINNAMON_TRAY_MANAGER_H__ */
