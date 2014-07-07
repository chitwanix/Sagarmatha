/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_DOC_SYSTEM_H__
#define __SAGARMATHA_DOC_SYSTEM_H__

#include <gio/gio.h>
#include <gtk/gtk.h>

#define SAGARMATHA_TYPE_DOC_SYSTEM                 (sagarmatha_doc_system_get_type ())
#define SAGARMATHA_DOC_SYSTEM(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), SAGARMATHA_TYPE_DOC_SYSTEM, SagarmathaDocSystem))
#define SAGARMATHA_DOC_SYSTEM_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_DOC_SYSTEM, SagarmathaDocSystemClass))
#define SAGARMATHA_IS_DOC_SYSTEM(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SAGARMATHA_TYPE_DOC_SYSTEM))
#define SAGARMATHA_IS_DOC_SYSTEM_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_DOC_SYSTEM))
#define SAGARMATHA_DOC_SYSTEM_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_DOC_SYSTEM, SagarmathaDocSystemClass))

typedef struct _SagarmathaDocSystem SagarmathaDocSystem;
typedef struct _SagarmathaDocSystemClass SagarmathaDocSystemClass;
typedef struct _SagarmathaDocSystemPrivate SagarmathaDocSystemPrivate;

struct _SagarmathaDocSystem
{
  GObject parent;

  SagarmathaDocSystemPrivate *priv;
};

struct _SagarmathaDocSystemClass
{
  GObjectClass parent_class;
};

GType sagarmatha_doc_system_get_type (void) G_GNUC_CONST;

SagarmathaDocSystem* sagarmatha_doc_system_get_default (void);

GSList *sagarmatha_doc_system_get_all (SagarmathaDocSystem    *system);

GtkRecentInfo *sagarmatha_doc_system_lookup_by_uri (SagarmathaDocSystem  *system,
                                               const char     *uri);

void sagarmatha_doc_system_queue_existence_check (SagarmathaDocSystem   *system,
                                             guint             n_items);

void sagarmatha_doc_system_open (SagarmathaDocSystem *system,
                            GtkRecentInfo  *info,
                            int             workspace);

#endif /* __SAGARMATHA_DOC_SYSTEM_H__ */
