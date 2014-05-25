/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __CINNAMON_EMBEDDED_WINDOW_PRIVATE_H__
#define __CINNAMON_EMBEDDED_WINDOW_PRIVATE_H__

#include "sagarmatha-embedded-window.h"
#include "sagarmatha-gtk-embed.h"

void _sagarmatha_embedded_window_set_actor (SagarmathaEmbeddedWindow      *window,
				       SagarmathaGtkEmbed            *embed);

void _sagarmatha_embedded_window_allocate (SagarmathaEmbeddedWindow *window,
				      int                  x,
				      int                  y,
				      int                  width,
				      int                  height);

void _sagarmatha_embedded_window_realize   (SagarmathaEmbeddedWindow *window);
void _sagarmatha_embedded_window_unrealize (SagarmathaEmbeddedWindow *window);

#endif /* __CINNAMON_EMBEDDED_WINDOW_PRIVATE_H__ */
