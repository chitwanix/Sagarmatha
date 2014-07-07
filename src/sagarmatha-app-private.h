/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_APP_PRIVATE_H__
#define __SAGARMATHA_APP_PRIVATE_H__

#include "sagarmatha-app.h"
#include "sagarmatha-app-system.h"

#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn.h>

G_BEGIN_DECLS

SagarmathaApp* _sagarmatha_app_new_for_window (MetaWindow *window);

SagarmathaApp* _sagarmatha_app_new (GMenuTreeEntry *entry);

void _sagarmatha_app_set_entry (SagarmathaApp *app, GMenuTreeEntry *entry);

void _sagarmatha_app_handle_startup_sequence (SagarmathaApp *app, SnStartupSequence *sequence);

void _sagarmatha_app_add_window (SagarmathaApp *app, MetaWindow *window);

void _sagarmatha_app_remove_window (SagarmathaApp *app, MetaWindow *window);

void _sagarmatha_app_do_match (SagarmathaApp         *app,
                          GSList           *terms,
                          GSList          **prefix_results,
                          GSList          **substring_results);

G_END_DECLS

#endif /* __SAGARMATHA_APP_PRIVATE_H__ */
