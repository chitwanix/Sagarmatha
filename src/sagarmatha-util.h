/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#ifndef __CINNAMON_UTIL_H__
#define __CINNAMON_UTIL_H__

#include <gio/gio.h>
#include <clutter/clutter.h>
#include <libsoup/soup.h>

G_BEGIN_DECLS

char    *sagarmatha_util_get_label_for_uri          (const char       *text_uri);
GIcon   *sagarmatha_util_get_icon_for_uri           (const char       *text_uri);

void     sagarmatha_util_set_hidden_from_pick       (ClutterActor     *actor,
                                                gboolean          hidden);

void     sagarmatha_util_get_transformed_allocation (ClutterActor     *actor,
                                                ClutterActorBox  *box);

int      sagarmatha_util_get_week_start             (void);

char    *sagarmatha_util_normalize_and_casefold     (const char       *str);

char    *sagarmatha_util_format_date                (const char       *format,
                                                gint64            time_ms);

ClutterModifierType
         sagarmatha_get_event_state                 (ClutterEvent     *event);

void     sagarmatha_write_soup_message_to_stream    (GOutputStream    *stream,
                                                SoupMessage      *message,
                                                GError          **error);

gboolean sagarmatha_write_string_to_stream          (GOutputStream    *stream,
                                                const char       *str,
                                                GError          **error);

char    *sagarmatha_get_file_contents_utf8_sync     (const char       *path,
                                                GError          **error);

void     sagarmatha_breakpoint                      (void);

gboolean sagarmatha_parse_search_provider           (const char       *data,
                                                char            **name,
                                                char            **url,
                                                GList           **langs,
                                                char            **icon_data_uri,
                                                GError          **error);

void sagarmatha_shader_effect_set_double_uniform (ClutterShaderEffect *effect,
                                             const gchar         *name,
                                             gdouble             value);

G_END_DECLS

#endif /* __CINNAMON_UTIL_H__ */
