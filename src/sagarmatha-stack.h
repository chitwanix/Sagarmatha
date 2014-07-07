/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_STACK_H__
#define __SAGARMATHA_STACK_H__

#include "st.h"
#include <gtk/gtk.h>

#define SAGARMATHA_TYPE_STACK                 (sagarmatha_stack_get_type ())
#define SAGARMATHA_STACK(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), SAGARMATHA_TYPE_STACK, SagarmathaStack))
#define SAGARMATHA_STACK_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_STACK, SagarmathaStackClass))
#define SAGARMATHA_IS_STACK(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SAGARMATHA_TYPE_STACK))
#define SAGARMATHA_IS_STACK_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_STACK))
#define SAGARMATHA_STACK_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_STACK, SagarmathaStackClass))

typedef struct _SagarmathaStack        SagarmathaStack;
typedef struct _SagarmathaStackClass   SagarmathaStackClass;

typedef struct _SagarmathaStackPrivate SagarmathaStackPrivate;

struct _SagarmathaStack
{
    StContainer parent;

    SagarmathaStackPrivate *priv;
};

struct _SagarmathaStackClass
{
    StContainerClass parent_class;
};

GType sagarmatha_stack_get_type (void) G_GNUC_CONST;

#endif /* __SAGARMATHA_STACK_H__ */
