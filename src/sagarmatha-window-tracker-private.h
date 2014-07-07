/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_WINDOW_TRACKER_PRIVATE_H__
#define __SAGARMATHA_WINDOW_TRACKER_PRIVATE_H__

#include "sagarmatha-window-tracker.h"

void _sagarmatha_window_tracker_add_child_process_app (SagarmathaWindowTracker *tracker,
                                                  GPid                pid,
                                                  SagarmathaApp           *app);

#endif
