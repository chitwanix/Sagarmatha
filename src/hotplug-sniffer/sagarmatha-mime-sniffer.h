/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * Copyright (C) 2011 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Suite 500, Boston, MA
 * 02110-1335, USA.
 *
 * Author: Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef __SAGARMATHA_MIME_SNIFFER_H__
#define __SAGARMATHA_MIME_SNIFFER_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define SAGARMATHA_TYPE_MIME_SNIFFER            (sagarmatha_mime_sniffer_get_type ())
#define SAGARMATHA_MIME_SNIFFER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SAGARMATHA_TYPE_MIME_SNIFFER, SagarmathaMimeSniffer))
#define SAGARMATHA_IS_MIME_SNIFFER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SAGARMATHA_TYPE_MIME_SNIFFER))
#define SAGARMATHA_MIME_SNIFFER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  SAGARMATHA_TYPE_MIME_SNIFFER, SagarmathaMimeSnifferClass))
#define SAGARMATHA_IS_MIME_SNIFFER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  SAGARMATHA_TYPE_MIME_SNIFFER))
#define SAGARMATHA_MIME_SNIFFER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  SAGARMATHA_TYPE_MIME_SNIFFER, SagarmathaMimeSnifferClass))

typedef struct _SagarmathaMimeSniffer          SagarmathaMimeSniffer;
typedef struct _SagarmathaMimeSnifferPrivate   SagarmathaMimeSnifferPrivate;
typedef struct _SagarmathaMimeSnifferClass     SagarmathaMimeSnifferClass;

struct _SagarmathaMimeSniffer
{
  GObject parent_instance;

  SagarmathaMimeSnifferPrivate *priv;
};

struct _SagarmathaMimeSnifferClass
{
  GObjectClass parent_class;
};

GType    sagarmatha_mime_sniffer_get_type     (void) G_GNUC_CONST;

SagarmathaMimeSniffer *sagarmatha_mime_sniffer_new (GFile *file);

void sagarmatha_mime_sniffer_sniff_async (SagarmathaMimeSniffer *self,
                                     GAsyncReadyCallback callback,
                                     gpointer user_data);

gchar ** sagarmatha_mime_sniffer_sniff_finish (SagarmathaMimeSniffer *self,
                                          GAsyncResult *res,
                                          GError **error);

G_END_DECLS

#endif /* __SAGARMATHA_MIME_SNIFFER_H__ */
