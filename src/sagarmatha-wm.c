/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#include "config.h"

#include <string.h>

#include <meta/keybindings.h>

#include "sagarmatha-wm-private.h"
#include "sagarmatha-global.h"
#include "sagarmatha-marshal.h"

struct _SagarmathaWM {
  GObject parent;

  MetaPlugin *plugin;
};

/* Signals */
enum
{
  MINIMIZE,
  MAXIMIZE,
  UNMAXIMIZE,
  TILE,
  MAP,
  DESTROY,
  SWITCH_WORKSPACE,
  KILL_SWITCH_WORKSPACE,
  KILL_WINDOW_EFFECTS,

  LAST_SIGNAL
};

G_DEFINE_TYPE(SagarmathaWM, sagarmatha_wm, G_TYPE_OBJECT);

static guint sagarmatha_wm_signals [LAST_SIGNAL] = { 0 };

static void
sagarmatha_wm_init (SagarmathaWM *wm)
{
}

static void
sagarmatha_wm_finalize (GObject *object)
{
  G_OBJECT_CLASS (sagarmatha_wm_parent_class)->finalize (object);
}

static void
sagarmatha_wm_class_init (SagarmathaWMClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = sagarmatha_wm_finalize;

  sagarmatha_wm_signals[MINIMIZE] =
    g_signal_new ("minimize",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  META_TYPE_WINDOW_ACTOR);
  sagarmatha_wm_signals[MAXIMIZE] =
    g_signal_new ("maximize",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  _sagarmatha_marshal_VOID__OBJECT_INT_INT_INT_INT,
                  G_TYPE_NONE, 5,
                  META_TYPE_WINDOW_ACTOR, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
  sagarmatha_wm_signals[UNMAXIMIZE] =
    g_signal_new ("unmaximize",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  _sagarmatha_marshal_VOID__OBJECT_INT_INT_INT_INT,
                  G_TYPE_NONE, 5,
                  META_TYPE_WINDOW_ACTOR, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
  sagarmatha_wm_signals[TILE] =
    g_signal_new ("tile",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  _sagarmatha_marshal_VOID__OBJECT_INT_INT_INT_INT,
                  G_TYPE_NONE, 5,
                  META_TYPE_WINDOW_ACTOR, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
  sagarmatha_wm_signals[MAP] =
    g_signal_new ("map",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  META_TYPE_WINDOW_ACTOR);
  sagarmatha_wm_signals[DESTROY] =
    g_signal_new ("destroy",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  META_TYPE_WINDOW_ACTOR);
  sagarmatha_wm_signals[SWITCH_WORKSPACE] =
    g_signal_new ("switch-workspace",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL,
		  _sagarmatha_marshal_VOID__INT_INT_INT,
		  G_TYPE_NONE, 3,
                  G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
  sagarmatha_wm_signals[KILL_SWITCH_WORKSPACE] =
    g_signal_new ("kill-switch-workspace",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  sagarmatha_wm_signals[KILL_WINDOW_EFFECTS] =
    g_signal_new ("kill-window-effects",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL,
		  g_cclosure_marshal_VOID__OBJECT,
		  G_TYPE_NONE, 1,
		  META_TYPE_WINDOW_ACTOR);
}

void
_sagarmatha_wm_switch_workspace (SagarmathaWM      *wm,
                            gint          from,
                            gint          to,
                            MetaMotionDirection direction)
{
  g_signal_emit (wm, sagarmatha_wm_signals[SWITCH_WORKSPACE], 0,
                 from, to, direction);
}

/**
 * sagarmatha_wm_completed_switch_workspace:
 * @wm: the SagarmathaWM
 *
 * The plugin must call this when it has finished switching the
 * workspace.
 **/
void
sagarmatha_wm_completed_switch_workspace (SagarmathaWM *wm)
{
  meta_plugin_switch_workspace_completed (wm->plugin);
}

/**
 * sagarmatha_wm_completed_minimize:
 * @wm: the SagarmathaWM
 * @actor: the MetaWindowActor actor
 *
 * The plugin must call this when it has completed a window minimize effect.
 **/
void
sagarmatha_wm_completed_minimize (SagarmathaWM         *wm,
                             MetaWindowActor *actor)
{
  meta_plugin_minimize_completed (wm->plugin, actor);
}

/**
 * sagarmatha_wm_completed_maximize:
 * @wm: the SagarmathaWM
 * @actor: the MetaWindowActor actor
 *
 * The plugin must call this when it has completed a window maximize effect.
 **/
void
sagarmatha_wm_completed_maximize (SagarmathaWM         *wm,
                             MetaWindowActor *actor)
{
  meta_plugin_maximize_completed (wm->plugin, actor);
}

/**
 * sagarmatha_wm_completed_tile:
 * @wm: the SagarmathaWM
 * @actor: the MetaWindowActor actor
 *
 * The plugin must call this when it has completed a window tile effect.
 **/
void
sagarmatha_wm_completed_tile  (SagarmathaWM         *wm,
                             MetaWindowActor *actor)
{
  meta_plugin_tile_completed (wm->plugin, actor);
}


/**
 * sagarmatha_wm_completed_unmaximize:
 * @wm: the SagarmathaWM
 * @actor: the MetaWindowActor actor
 *
 * The plugin must call this when it has completed a window unmaximize effect.
 **/
void
sagarmatha_wm_completed_unmaximize (SagarmathaWM         *wm,
                               MetaWindowActor *actor)
{
  meta_plugin_unmaximize_completed (wm->plugin, actor);
}

/**
 * sagarmatha_wm_completed_map:
 * @wm: the SagarmathaWM
 * @actor: the MetaWindowActor actor
 *
 * The plugin must call this when it has completed a window map effect.
 **/
void
sagarmatha_wm_completed_map (SagarmathaWM         *wm,
                        MetaWindowActor *actor)
{
  meta_plugin_map_completed (wm->plugin, actor);
}

/**
 * sagarmatha_wm_completed_destroy:
 * @wm: the SagarmathaWM
 * @actor: the MetaWindowActor actor
 *
 * The plugin must call this when it has completed a window destroy effect.
 **/
void
sagarmatha_wm_completed_destroy (SagarmathaWM         *wm,
                            MetaWindowActor *actor)
{
  meta_plugin_destroy_completed (wm->plugin, actor);
}

void
_sagarmatha_wm_kill_switch_workspace (SagarmathaWM      *wm)
{
  g_signal_emit (wm, sagarmatha_wm_signals[KILL_SWITCH_WORKSPACE], 0);
}

void
_sagarmatha_wm_kill_window_effects (SagarmathaWM         *wm,
                               MetaWindowActor *actor)
{
  g_signal_emit (wm, sagarmatha_wm_signals[KILL_WINDOW_EFFECTS], 0, actor);
}


void
_sagarmatha_wm_minimize (SagarmathaWM         *wm,
                    MetaWindowActor *actor)
{
  g_signal_emit (wm, sagarmatha_wm_signals[MINIMIZE], 0, actor);
}

void
_sagarmatha_wm_maximize (SagarmathaWM         *wm,
                    MetaWindowActor *actor,
                    int              target_x,
                    int              target_y,
                    int              target_width,
                    int              target_height)
{
  g_signal_emit (wm, sagarmatha_wm_signals[MAXIMIZE], 0, actor, target_x, target_y, target_width, target_height);
}

void
_sagarmatha_wm_unmaximize (SagarmathaWM         *wm,
                      MetaWindowActor *actor,
                      int              target_x,
                      int              target_y,
                      int              target_width,
                      int              target_height)
{
  g_signal_emit (wm, sagarmatha_wm_signals[UNMAXIMIZE], 0, actor, target_x, target_y, target_width, target_height);
}

void
_sagarmatha_wm_tile (SagarmathaWM         *wm,
                   MetaWindowActor    *actor,
                   int                 target_x,
                   int                 target_y,
                   int                 target_width,
                   int                 target_height)
{
  g_signal_emit (wm, sagarmatha_wm_signals[TILE], 0, actor, target_x, target_y, target_width, target_height);
}

void
_sagarmatha_wm_map (SagarmathaWM         *wm,
               MetaWindowActor *actor)
{
  g_signal_emit (wm, sagarmatha_wm_signals[MAP], 0, actor);
}

void
_sagarmatha_wm_destroy (SagarmathaWM         *wm,
                   MetaWindowActor *actor)
{
  g_signal_emit (wm, sagarmatha_wm_signals[DESTROY], 0, actor);
}

/**
 * sagarmatha_wm_new:
 * @plugin: the #MetaPlugin
 *
 * Creates a new window management interface by hooking into @plugin.
 *
 * Return value: the new window-management interface
 **/
SagarmathaWM *
sagarmatha_wm_new (MetaPlugin *plugin)
{
  SagarmathaWM *wm;

  wm = g_object_new (SAGARMATHA_TYPE_WM, NULL);
  wm->plugin = plugin;

  return wm;
}

