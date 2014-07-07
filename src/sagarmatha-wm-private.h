/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_WM_PRIVATE_H__
#define __SAGARMATHA_WM_PRIVATE_H__

#include "sagarmatha-wm.h"

G_BEGIN_DECLS

/* These forward along the different effects from SagarmathaPlugin */

void _sagarmatha_wm_minimize   (SagarmathaWM         *wm,
                           MetaWindowActor *actor);
void _sagarmatha_wm_maximize   (SagarmathaWM         *wm,
                           MetaWindowActor *actor,
                           gint             x,
                           gint             y,
                           gint             width,
                           gint             height);
void _sagarmatha_wm_unmaximize (SagarmathaWM         *wm,
                           MetaWindowActor *actor,
                           gint             x,
                           gint             y,
                           gint             width,
                           gint             height);
void _sagarmatha_wm_tile       (SagarmathaWM         *wm,
                           MetaWindowActor *actor,
                           gint             x,
                           gint             y,
                           gint             width,
                           gint             height);
void _sagarmatha_wm_map        (SagarmathaWM         *wm,
                           MetaWindowActor *actor);
void _sagarmatha_wm_destroy    (SagarmathaWM         *wm,
                           MetaWindowActor *actor);

void _sagarmatha_wm_switch_workspace      (SagarmathaWM             *wm,
                                      gint                 from,
                                      gint                 to,
                                      MetaMotionDirection  direction);
void _sagarmatha_wm_kill_window_effects   (SagarmathaWM             *wm,
                                      MetaWindowActor     *actor);
void _sagarmatha_wm_kill_switch_workspace (SagarmathaWM             *wm);

G_END_DECLS

#endif /* __SAGARMATHA_WM_PRIVATE_H__ */
