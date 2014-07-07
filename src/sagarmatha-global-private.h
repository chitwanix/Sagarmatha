/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_GLOBAL_PRIVATE_H__
#define __SAGARMATHA_GLOBAL_PRIVATE_H__

#include "sagarmatha-global.h"

#include <cjs/gjs.h>

void _sagarmatha_global_init            (const char *first_property_name,
                                    ...);
void _sagarmatha_global_set_plugin      (SagarmathaGlobal  *global,
                                    MetaPlugin   *plugin);

GjsContext *_sagarmatha_global_get_gjs_context (SagarmathaGlobal  *global);

gboolean _sagarmatha_global_check_xdnd_event (SagarmathaGlobal  *global,
                                         XEvent       *xev);

#endif /* __SAGARMATHA_GLOBAL_PRIVATE_H__ */
