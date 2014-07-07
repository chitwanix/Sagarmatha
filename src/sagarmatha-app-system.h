/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_APP_SYSTEM_H__
#define __SAGARMATHA_APP_SYSTEM_H__

#include <gio/gio.h>
#include <clutter/clutter.h>
#include <meta/window.h>
#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#include <gmenu-tree.h>

#include "sagarmatha-app.h"

#define SAGARMATHA_TYPE_APP_SYSTEM                 (sagarmatha_app_system_get_type ())
#define SAGARMATHA_APP_SYSTEM(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), SAGARMATHA_TYPE_APP_SYSTEM, SagarmathaAppSystem))
#define SAGARMATHA_APP_SYSTEM_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_APP_SYSTEM, SagarmathaAppSystemClass))
#define SAGARMATHA_IS_APP_SYSTEM(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SAGARMATHA_TYPE_APP_SYSTEM))
#define SAGARMATHA_IS_APP_SYSTEM_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_APP_SYSTEM))
#define SAGARMATHA_APP_SYSTEM_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_APP_SYSTEM, SagarmathaAppSystemClass))

typedef struct _SagarmathaAppSystem SagarmathaAppSystem;
typedef struct _SagarmathaAppSystemClass SagarmathaAppSystemClass;
typedef struct _SagarmathaAppSystemPrivate SagarmathaAppSystemPrivate;

struct _SagarmathaAppSystem
{
  GObject parent;

  SagarmathaAppSystemPrivate *priv;
};

struct _SagarmathaAppSystemClass
{
  GObjectClass parent_class;

  void (*installed_changed)(SagarmathaAppSystem *appsys, gpointer user_data);
  void (*favorites_changed)(SagarmathaAppSystem *appsys, gpointer user_data);
};

GType           sagarmatha_app_system_get_type    (void) G_GNUC_CONST;
SagarmathaAppSystem *sagarmatha_app_system_get_default (void);

GMenuTree      *sagarmatha_app_system_get_tree                     (SagarmathaAppSystem *system);

SagarmathaApp       *sagarmatha_app_system_lookup_app                   (SagarmathaAppSystem  *system,
                                                               const char      *id);
SagarmathaApp       *sagarmatha_app_system_lookup_settings_app          (SagarmathaAppSystem  *system,
                                                               const char      *id);


SagarmathaApp       *sagarmatha_app_system_lookup_app_by_tree_entry     (SagarmathaAppSystem  *system,
                                                               GMenuTreeEntry  *entry);
SagarmathaApp       *sagarmatha_app_system_lookup_settings_app_by_tree_entry     (SagarmathaAppSystem  *system,
                                                               GMenuTreeEntry  *entry);
SagarmathaApp       *sagarmatha_app_system_lookup_app_for_path          (SagarmathaAppSystem  *system,
                                                               const char      *desktop_path);
SagarmathaApp       *sagarmatha_app_system_lookup_heuristic_basename    (SagarmathaAppSystem  *system,
                                                               const char      *id);


GSList         *sagarmatha_app_system_get_all                   (SagarmathaAppSystem  *system);

GSList         *sagarmatha_app_system_get_running               (SagarmathaAppSystem  *self);

GSList         *sagarmatha_app_system_initial_search            (SagarmathaAppSystem  *system,
                                                            GSList          *terms);
GSList         *sagarmatha_app_system_subsearch                 (SagarmathaAppSystem  *system,
                                                            GSList          *previous_results,
                                                            GSList          *terms);

GMenuTree      *sagarmatha_app_system_get_settings_tree         (SagarmathaAppSystem *system);

GSList         *sagarmatha_app_system_search_settings           (SagarmathaAppSystem *system,
                                                            GSList         *terms);
SagarmathaApp       *sagarmatha_app_system_lookup_setting            (SagarmathaAppSystem *system,
                                                            const char     *id);

#endif /* __SAGARMATHA_APP_SYSTEM_H__ */
