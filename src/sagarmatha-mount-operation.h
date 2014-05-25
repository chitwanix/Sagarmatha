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

#ifndef __CINNAMON_MOUNT_OPERATION_H__
#define __CINNAMON_MOUNT_OPERATION_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define CINNAMON_TYPE_MOUNT_OPERATION         (sagarmatha_mount_operation_get_type ())
#define CINNAMON_MOUNT_OPERATION(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), CINNAMON_TYPE_MOUNT_OPERATION, SagarmathaMountOperation))
#define CINNAMON_MOUNT_OPERATION_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), CINNAMON_TYPE_MOUNT_OPERATION, SagarmathaMountOperationClass))
#define CINNAMON_IS_MOUNT_OPERATION(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), CINNAMON_TYPE_MOUNT_OPERATION))
#define CINNAMON_IS_MOUNT_OPERATION_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), CINNAMON_TYPE_MOUNT_OPERATION))
#define CINNAMON_MOUNT_OPERATION_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), CINNAMON_TYPE_MOUNT_OPERATION, SagarmathaMountOperationClass))

typedef struct _SagarmathaMountOperation         SagarmathaMountOperation;
typedef struct _SagarmathaMountOperationClass    SagarmathaMountOperationClass;
typedef struct _SagarmathaMountOperationPrivate  SagarmathaMountOperationPrivate;

struct _SagarmathaMountOperation
{
  GMountOperation parent_instance;

  SagarmathaMountOperationPrivate *priv;
};

struct _SagarmathaMountOperationClass
{
  GMountOperationClass parent_class;
};


GType            sagarmatha_mount_operation_get_type   (void);
GMountOperation *sagarmatha_mount_operation_new        (void);

GArray * sagarmatha_mount_operation_get_show_processes_pids (SagarmathaMountOperation *self);
gchar ** sagarmatha_mount_operation_get_show_processes_choices (SagarmathaMountOperation *self);
gchar * sagarmatha_mount_operation_get_show_processes_message (SagarmathaMountOperation *self);

G_END_DECLS

#endif /* __CINNAMON_MOUNT_OPERATION_H__ */
