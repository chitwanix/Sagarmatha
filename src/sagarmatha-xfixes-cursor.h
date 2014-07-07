/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_XFIXES_CURSOR_H__
#define __SAGARMATHA_XFIXES_CURSOR_H__

#include <clutter/clutter.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

typedef struct _SagarmathaXFixesCursor        SagarmathaXFixesCursor;
typedef struct _SagarmathaXFixesCursorClass   SagarmathaXFixesCursorClass;

#define SAGARMATHA_TYPE_XFIXES_CURSOR             (sagarmatha_xfixes_cursor_get_type ())
#define SAGARMATHA_XFIXES_CURSOR(object)          (G_TYPE_CHECK_INSTANCE_CAST ((object), SAGARMATHA_TYPE_XFIXES_CURSOR, SagarmathaXFixesCursor))
#define SAGARMATHA_XFIXES_CURSOR_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_XFIXES_CURSOR, SagarmathaXFixesCursorClass))
#define SAGARMATHA_IS_XFIXES_CURSOR(object)       (G_TYPE_CHECK_INSTANCE_TYPE ((object), SAGARMATHA_TYPE_XFIXES_CURSOR))
#define SAGARMATHA_IS_XFIXES_CURSOR_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_XFIXES_CURSOR))
#define SAGARMATHA_XFIXES_CURSOR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_XFIXES_CURSOR, SagarmathaXFixesCursorClass))

GType               sagarmatha_xfixes_cursor_get_type     (void) G_GNUC_CONST;

SagarmathaXFixesCursor   *sagarmatha_xfixes_cursor_get_for_stage (ClutterStage *stage);

void                sagarmatha_xfixes_cursor_show (SagarmathaXFixesCursor *xfixes_cursor);
void                sagarmatha_xfixes_cursor_hide (SagarmathaXFixesCursor *xfixes_cursor);
int                 sagarmatha_xfixes_cursor_get_hot_x (SagarmathaXFixesCursor *xfixes_cursor);
int                 sagarmatha_xfixes_cursor_get_hot_y (SagarmathaXFixesCursor *xfixes_cursor);
void                sagarmatha_xfixes_cursor_update_texture_image (SagarmathaXFixesCursor *xfixes_cursor,
                                                              ClutterTexture *texture);

G_END_DECLS

#endif /* __SAGARMATHA_XFIXES_CURSOR_H__ */
