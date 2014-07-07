/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_RECORDER_SRC_H__
#define __SAGARMATHA_RECORDER_SRC_H__

#include <gst/gst.h>

G_BEGIN_DECLS

/**
 * SagarmathaRecorderSrc:
 *
 * sagarmatharecordersrc a custom source element is pretty much like a very
 * simple version of the stander GStreamer 'appsrc' element, without
 * any of the provisions for seeking, generating data on demand,
 * etc. In both cases, the application supplies the buffers and the
 * element pushes them into the pipeline. The main reason for not using
 * appsrc is that it wasn't a supported element until gstreamer 0.10.22,
 * and as of 2009-03, many systems still have 0.10.21.
 */
typedef struct _SagarmathaRecorderSrc      SagarmathaRecorderSrc;
typedef struct _SagarmathaRecorderSrcClass SagarmathaRecorderSrcClass;

#define SAGARMATHA_TYPE_RECORDER_SRC              (sagarmatha_recorder_src_get_type ())
#define SAGARMATHA_RECORDER_SRC(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), SAGARMATHA_TYPE_RECORDER_SRC, SagarmathaRecorderSrc))
#define SAGARMATHA_RECORDER_SRC_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_RECORDER_SRC, SagarmathaRecorderSrcClass))
#define SAGARMATHA_IS_RECORDER_SRC(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), SAGARMATHA_TYPE_RECORDER_SRC))
#define SAGARMATHA_IS_RECORDER_SRC_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_RECORDER_SRC))
#define SAGARMATHA_RECORDER_SRC_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_RECORDER_SRC, SagarmathaRecorderSrcClass))

GType              sagarmatha_recorder_src_get_type     (void) G_GNUC_CONST;

void sagarmatha_recorder_src_register (void);

void sagarmatha_recorder_src_add_buffer (SagarmathaRecorderSrc *src,
				    GstBuffer        *buffer);
void sagarmatha_recorder_src_close      (SagarmathaRecorderSrc *src);

G_END_DECLS

#endif /* __SAGARMATHA_RECORDER_SRC_H__ */
