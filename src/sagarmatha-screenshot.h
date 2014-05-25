/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __CINNAMON_SCREENSHOT_H__
#define __CINNAMON_SCREENSHOT_H__

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

#define CINNAMON_TYPE_SCREENSHOT              (sagarmatha_screenshot_get_type ())
#define CINNAMON_SCREENSHOT(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), CINNAMON_TYPE_SCREENSHOT, SagarmathaScreenshot))
#define CINNAMON_SCREENSHOT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), CINNAMON_TYPE_SCREENSHOT, SagarmathaScreenshotClass))
#define CINNAMON_IS_SCREENSHOT(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), CINNAMON_TYPE_SCREENSHOT))
#define CINNAMON_IS_SCREENSHOT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), CINNAMON_TYPE_SCREENSHOT))
#define CINNAMON_SCREENSHOT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), CINNAMON_TYPE_SCREENSHOT, SagarmathaScreenshotClass))

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

#endif /* ___CINNAMON_SCREENSHOT_H__ */
