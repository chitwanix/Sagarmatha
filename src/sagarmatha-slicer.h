/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __CINNAMON_SLICER_H__
#define __CINNAMON_SLICER_H__

#include "st.h"

#define CINNAMON_TYPE_SLICER                 (sagarmatha_slicer_get_type ())
#define CINNAMON_SLICER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), CINNAMON_TYPE_SLICER, SagarmathaSlicer))
#define CINNAMON_SLICER_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), CINNAMON_TYPE_SLICER, SagarmathaSlicerClass))
#define CINNAMON_IS_SLICER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CINNAMON_TYPE_SLICER))
#define CINNAMON_IS_SLICER_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), CINNAMON_TYPE_SLICER))
#define CINNAMON_SLICER_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), CINNAMON_TYPE_SLICER, SagarmathaSlicerClass))

typedef struct _SagarmathaSlicer        SagarmathaSlicer;
typedef struct _SagarmathaSlicerClass   SagarmathaSlicerClass;

typedef struct _SagarmathaSlicerPrivate SagarmathaSlicerPrivate;

struct _SagarmathaSlicer
{
    StBin parent;

    SagarmathaSlicerPrivate *priv;
};

struct _SagarmathaSlicerClass
{
    StBinClass parent_class;
};

GType sagarmatha_slicer_get_type (void) G_GNUC_CONST;

#endif /* __CINNAMON_SLICER_H__ */
