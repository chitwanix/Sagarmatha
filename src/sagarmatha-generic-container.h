/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_GENERIC_CONTAINER_H__
#define __SAGARMATHA_GENERIC_CONTAINER_H__

#include "st.h"

#define SAGARMATHA_TYPE_GENERIC_CONTAINER                 (sagarmatha_generic_container_get_type ())
#define SAGARMATHA_GENERIC_CONTAINER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), SAGARMATHA_TYPE_GENERIC_CONTAINER, SagarmathaGenericContainer))
#define SAGARMATHA_GENERIC_CONTAINER_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_GENERIC_CONTAINER, SagarmathaGenericContainerClass))
#define SAGARMATHA_IS_GENERIC_CONTAINER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SAGARMATHA_TYPE_GENERIC_CONTAINER))
#define SAGARMATHA_IS_GENERIC_CONTAINER_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_GENERIC_CONTAINER))
#define SAGARMATHA_GENERIC_CONTAINER_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_GENERIC_CONTAINER, SagarmathaGenericContainerClass))

typedef struct {
  float min_size;
  float natural_size;

  /* <private> */
  guint _refcount;
} SagarmathaGenericContainerAllocation;

#define SAGARMATHA_TYPE_GENERIC_CONTAINER_ALLOCATION (sagarmatha_generic_container_allocation_get_type ())
GType sagarmatha_generic_container_allocation_get_type (void);

typedef struct _SagarmathaGenericContainer        SagarmathaGenericContainer;
typedef struct _SagarmathaGenericContainerClass   SagarmathaGenericContainerClass;

typedef struct _SagarmathaGenericContainerPrivate SagarmathaGenericContainerPrivate;

struct _SagarmathaGenericContainer
{
    StContainer parent;

    SagarmathaGenericContainerPrivate *priv;
};

struct _SagarmathaGenericContainerClass
{
    StContainerClass parent_class;
};

GType    sagarmatha_generic_container_get_type         (void) G_GNUC_CONST;

guint    sagarmatha_generic_container_get_n_skip_paint (SagarmathaGenericContainer *self);

gboolean sagarmatha_generic_container_get_skip_paint   (SagarmathaGenericContainer *self,
                                                   ClutterActor          *child);
void     sagarmatha_generic_container_set_skip_paint   (SagarmathaGenericContainer *self,
                                                   ClutterActor          *child,
                                                   gboolean               skip);

#endif /* __SAGARMATHA_GENERIC_CONTAINER_H__ */
