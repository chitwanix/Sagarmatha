/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_RECORDER_H__
#define __SAGARMATHA_RECORDER_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

/**
 * SECTION:SagarmathaRecorder
 * short_description: Record from a #ClutterStage
 *
 * The #SagarmathaRecorder object is used to make recordings ("screencasts")
 * of a #ClutterStage. Recording is done via #GStreamer. The default is
 * to encode as a Theora movie and write it to a file in the current
 * directory named after the date, but the encoding and output can
 * be configured.
 */
typedef struct _SagarmathaRecorder      SagarmathaRecorder;
typedef struct _SagarmathaRecorderClass SagarmathaRecorderClass;

#define SAGARMATHA_TYPE_RECORDER              (sagarmatha_recorder_get_type ())
#define SAGARMATHA_RECORDER(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), SAGARMATHA_TYPE_RECORDER, SagarmathaRecorder))
#define SAGARMATHA_RECORDER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_RECORDER, SagarmathaRecorderClass))
#define SAGARMATHA_IS_RECORDER(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), SAGARMATHA_TYPE_RECORDER))
#define SAGARMATHA_IS_RECORDER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_RECORDER))
#define SAGARMATHA_RECORDER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_RECORDER, SagarmathaRecorderClass))

GType              sagarmatha_recorder_get_type     (void) G_GNUC_CONST;

SagarmathaRecorder     *sagarmatha_recorder_new (ClutterStage  *stage);

void               sagarmatha_recorder_set_framerate (SagarmathaRecorder *recorder,
                                                 int framerate);
void               sagarmatha_recorder_set_filename (SagarmathaRecorder *recorder,
						const char    *filename);
void               sagarmatha_recorder_set_pipeline (SagarmathaRecorder *recorder,
						const char    *pipeline);
gboolean           sagarmatha_recorder_record       (SagarmathaRecorder *recorder);
void               sagarmatha_recorder_close        (SagarmathaRecorder *recorder);
void               sagarmatha_recorder_pause        (SagarmathaRecorder *recorder);
gboolean           sagarmatha_recorder_is_recording (SagarmathaRecorder *recorder);

G_END_DECLS

#endif /* __SAGARMATHA_RECORDER_H__ */
