/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_WM_H__
#define __SAGARMATHA_WM_H__

#include <glib-object.h>
#include <meta/meta-plugin.h>

G_BEGIN_DECLS

typedef struct _SagarmathaWM      SagarmathaWM;
typedef struct _SagarmathaWMClass SagarmathaWMClass;

#define SAGARMATHA_TYPE_WM              (sagarmatha_wm_get_type ())
#define SAGARMATHA_WM(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), SAGARMATHA_TYPE_WM, SagarmathaWM))
#define SAGARMATHA_WM_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_WM, SagarmathaWMClass))
#define SAGARMATHA_IS_WM(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), SAGARMATHA_TYPE_WM))
#define SAGARMATHA_IS_WM_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_WM))
#define SAGARMATHA_WM_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_WM, SagarmathaWMClass))

struct _SagarmathaWMClass
{
  GObjectClass parent_class;

};

GType    sagarmatha_wm_get_type                    (void) G_GNUC_CONST;

SagarmathaWM *sagarmatha_wm_new                        (MetaPlugin      *plugin);

void     sagarmatha_wm_completed_minimize         (SagarmathaWM         *wm,
                                              MetaWindowActor *actor);
void     sagarmatha_wm_completed_maximize         (SagarmathaWM         *wm,
                                              MetaWindowActor *actor);
void     sagarmatha_wm_completed_tile             (SagarmathaWM         *wm,
                                              MetaWindowActor *actor);
void     sagarmatha_wm_completed_unmaximize       (SagarmathaWM         *wm,
                                              MetaWindowActor *actor);
void     sagarmatha_wm_completed_map              (SagarmathaWM         *wm,
                                              MetaWindowActor *actor);
void     sagarmatha_wm_completed_destroy          (SagarmathaWM         *wm,
                                              MetaWindowActor *actor);
void     sagarmatha_wm_completed_switch_workspace (SagarmathaWM         *wm);

G_END_DECLS

#endif /* __SAGARMATHA_WM_H__ */
