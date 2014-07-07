/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __SAGARMATHA_PERF_LOG_H__
#define __SAGARMATHA_PERF_LOG_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _SagarmathaPerfLog SagarmathaPerfLog;
typedef struct _SagarmathaPerfLogClass SagarmathaPerfLogClass;

#define SAGARMATHA_TYPE_PERF_LOG              (sagarmatha_perf_log_get_type ())
#define SAGARMATHA_PERF_LOG(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), SAGARMATHA_TYPE_PERF_LOG, SagarmathaPerfLog))
#define SAGARMATHA_PERF_LOG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SAGARMATHA_TYPE_PERF_LOG, SagarmathaPerfLogClass))
#define SAGARMATHA_IS_PERF_LOG(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), SAGARMATHA_TYPE_PERF_LOG))
#define SAGARMATHA_IS_PERF_LOG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SAGARMATHA_TYPE_PERF_LOG))
#define SAGARMATHA_PERF_LOG_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SAGARMATHA_TYPE_PERF_LOG, SagarmathaPerfLogClass))

GType sagarmatha_perf_log_get_type (void) G_GNUC_CONST;

SagarmathaPerfLog *sagarmatha_perf_log_get_default (void);

void sagarmatha_perf_log_set_enabled (SagarmathaPerfLog *perf_log,
				 gboolean      enabled);

void sagarmatha_perf_log_define_event (SagarmathaPerfLog *perf_log,
				  const char   *name,
				  const char   *description,
				  const char   *signature);
void sagarmatha_perf_log_event        (SagarmathaPerfLog *perf_log,
				  const char   *name);
void sagarmatha_perf_log_event_i      (SagarmathaPerfLog *perf_log,
				  const char   *name,
				  gint32        arg);
void sagarmatha_perf_log_event_x      (SagarmathaPerfLog *perf_log,
				  const char   *name,
				  gint64        arg);
void sagarmatha_perf_log_event_s      (SagarmathaPerfLog *perf_log,
				  const char   *name,
				  const char   *arg);

void sagarmatha_perf_log_define_statistic (SagarmathaPerfLog *perf_log,
                                      const char   *name,
                                      const char   *description,
                                      const char   *signature);

void sagarmatha_perf_log_update_statistic_i (SagarmathaPerfLog *perf_log,
                                        const char   *name,
                                        int           value);
void sagarmatha_perf_log_update_statistic_x (SagarmathaPerfLog *perf_log,
                                        const char   *name,
                                        gint64        value);

typedef void (*SagarmathaPerfStatisticsCallback) (SagarmathaPerfLog *perf_log,
                                             gpointer      data);

void sagarmatha_perf_log_add_statistics_callback (SagarmathaPerfLog               *perf_log,
                                             SagarmathaPerfStatisticsCallback callback,
                                             gpointer                    user_data,
                                             GDestroyNotify              notify);

void sagarmatha_perf_log_collect_statistics (SagarmathaPerfLog *perf_log);

typedef void (*SagarmathaPerfReplayFunction) (gint64      time,
					 const char *name,
					 const char *signature,
					 GValue     *arg,
                                         gpointer    user_data);

void sagarmatha_perf_log_replay (SagarmathaPerfLog            *perf_log,
			    SagarmathaPerfReplayFunction  replay_function,
                            gpointer                 user_data);

gboolean sagarmatha_perf_log_dump_events (SagarmathaPerfLog   *perf_log,
                                     GOutputStream  *out,
                                     GError        **error);
gboolean sagarmatha_perf_log_dump_log    (SagarmathaPerfLog   *perf_log,
                                     GOutputStream  *out,
                                     GError        **error);

G_END_DECLS

#endif /* __SAGARMATHA_PERF_LOG_H__ */
