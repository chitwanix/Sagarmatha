/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __CINNAMON_SCREEN_GRABBER_H__
#define __CINNAMON_SCREEN_GRABBER_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SECTION:sagarmatha-screen-grabber
 * @short_description: Grab pixel data from the screen
 *
 * The #SagarmathaScreenGrabber object is used to download previous drawn
 * content to the screen. It internally uses pixel-buffer objects if
 * available, otherwise falls back to cogl_read_pixels().
 *
 * If you are repeatedly grabbing images of the same size from the
 * screen, it makes sense to create one #SagarmathaScreenGrabber and keep
 * it around. Otherwise, it's fine to simply create one as needed and
 * then get rid of it.
 */

typedef struct _SagarmathaScreenGrabber      SagarmathaScreenGrabber;
typedef struct _SagarmathaScreenGrabberClass SagarmathaScreenGrabberClass;

#define CINNAMON_TYPE_SCREEN_GRABBER              (sagarmatha_screen_grabber_get_type ())
#define CINNAMON_SCREEN_GRABBER(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), CINNAMON_TYPE_SCREEN_GRABBER, SagarmathaScreenGrabber))
#define CINNAMON_SCREEN_GRABBER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), CINNAMON_TYPE_SCREEN_GRABBER, SagarmathaScreenGrabberClass))
#define CINNAMON_IS_SCREEN_GRABBER(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), CINNAMON_TYPE_SCREEN_GRABBER))
#define CINNAMON_IS_SCREEN_GRABBER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), CINNAMON_TYPE_SCREEN_GRABBER))
#define CINNAMON_SCREEN_GRABBER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), CINNAMON_TYPE_SCREEN_GRABBER, SagarmathaScreenGrabberClass))

GType sagarmatha_screen_grabber_get_type (void) G_GNUC_CONST;

SagarmathaScreenGrabber *sagarmatha_screen_grabber_new  (void);
guchar *            sagarmatha_screen_grabber_grab (SagarmathaScreenGrabber *grabber,
                                               int                 x,
                                               int                 y,
                                               int                 width,
                                               int                 height);

G_END_DECLS

#endif /* __CINNAMON_SCREEN_GRABBER_H__ */
