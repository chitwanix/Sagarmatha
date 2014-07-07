/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_SCREENSHOT_H__
#define __SAGARMATHA_SCREENSHOT_H__

/**
 * SECTION:sagarmatha-screenshot
 * @short_description: Grabs screenshots of areas and/or windows
 *
 * The #SagarmathaScreenshot object is used to take screenshots of screen
 * areas or windows and write them out as png files.
 *
 */

typedef struct _SagarmathaScreenshot      SagarmathaScreenshot;
typedef struct _SagarmathaScreenshotClass SagarmathaScreenshotClass;

#define SAGARMATHA_TYPE_SCREENSHOT              (sagarmatha_screenshot_get_type ())
#define SAGARMATHA_SCREENSHOT(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), SAGARMATHA_TYPE_SCREENSHOT, SagarmathaScreenshot))
#define SAGARMATHA_SCREENSHOT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_SCREENSHOT, SagarmathaScreenshotClass))
#define SAGARMATHA_IS_SCREENSHOT(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), SAGARMATHA_TYPE_SCREENSHOT))
#define SAGARMATHA_IS_SCREENSHOT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_SCREENSHOT))
#define SAGARMATHA_SCREENSHOT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_SCREENSHOT, SagarmathaScreenshotClass))

GType sagarmatha_screenshot_get_type (void) G_GNUC_CONST;

SagarmathaScreenshot *sagarmatha_screenshot_new (void);

typedef void (*SagarmathaScreenshotCallback)  (SagarmathaScreenshot *screenshot,
                                           gboolean success,
                                           cairo_rectangle_int_t *screenshot_area);

void    sagarmatha_screenshot_screenshot_area      (SagarmathaScreenshot *screenshot,
                                                gboolean include_cursor,
                                                int x,
                                                int y,
                                                int width,
                                                int height,
                                                const char *filename,
                                                SagarmathaScreenshotCallback callback);

void    sagarmatha_screenshot_screenshot_window    (SagarmathaScreenshot *screenshot,
                                                gboolean include_frame,
                                                gboolean include_cursor,
                                                const char *filename,
                                                SagarmathaScreenshotCallback callback);

void    sagarmatha_screenshot_screenshot           (SagarmathaScreenshot *screenshot,
                                                gboolean include_cursor,
                                                const char *filename,
                                                SagarmathaScreenshotCallback callback);

#endif /* ___SAGARMATHA_SCREENSHOT_H__ */
