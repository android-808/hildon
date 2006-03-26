/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005 Nokia Corporation.
 *
 * Contact: Luc Pionchon <luc.pionchon@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef __HILDON_TIME_EDITOR_H__
#define __HILDON_TIME_EDITOR_H__

#include <gtk/gtkcontainer.h>

G_BEGIN_DECLS

#define HILDON_TYPE_TIME_EDITOR            (hildon_time_editor_get_type())

#define HILDON_TIME_EDITOR(obj)            (GTK_CHECK_CAST (obj,           HILDON_TYPE_TIME_EDITOR, HildonTimeEditor))
#define HILDON_TIME_EDITOR_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_TIME_EDITOR, HildonTimeEditorClass))

#define HILDON_IS_TIME_EDITOR(obj)         (GTK_CHECK_TYPE       (obj,     HILDON_TYPE_TIME_EDITOR))
#define HILDON_IS_TIME_EDITOR_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_TIME_EDITOR))


typedef struct _HildonTimeEditor      HildonTimeEditor;
typedef struct _HildonTimeEditorClass HildonTimeEditorClass;

struct _HildonTimeEditor {
    GtkContainer parent;
};

struct _HildonTimeEditorClass {
    GtkContainerClass parent_class;
};


GType      hildon_time_editor_get_type(void) G_GNUC_CONST;
GtkWidget *hildon_time_editor_new(void);


void       hildon_time_editor_set_time           (HildonTimeEditor * editor,
                                                  guint              hours,
                                                  guint              minutes,
                                                  guint              seconds);

void       hildon_time_editor_get_time           (HildonTimeEditor * editor,
                                                  guint            * hours,
                                                  guint            * minutes,
                                                  guint            * seconds);

#ifndef HILDON_DISABLE_DEPRECATED
void       hildon_time_editor_show_seconds         (HildonTimeEditor * editor, gboolean enable);
void       hildon_time_editor_enable_duration_mode (HildonTimeEditor * editor, gboolean enable);
#endif /* HILDON_DISABLE_DEPRECATED */


void       hildon_time_editor_set_duration_range (HildonTimeEditor * editor,
                                                  guint              min_seconds,
                                                  guint              max_seconds);

void       hildon_time_editor_get_duration_range (HildonTimeEditor * editor,
                                                  guint            * min_seconds,
                                                  guint            * max_seconds);

void       hildon_time_editor_set_ticks          (HildonTimeEditor * editor, guint    ticks);
guint      hildon_time_editor_get_ticks          (HildonTimeEditor * editor);

void       hildon_time_editor_set_show_seconds   (HildonTimeEditor * editor, gboolean show_seconds);
gboolean   hildon_time_editor_get_show_seconds   (HildonTimeEditor * editor);

void       hildon_time_editor_set_show_hours     (HildonTimeEditor * editor, gboolean show_hours);
gboolean   hildon_time_editor_get_show_hours     (HildonTimeEditor * editor);

void       hildon_time_editor_set_duration_mode  (HildonTimeEditor * editor, gboolean duration_mode);
gboolean   hildon_time_editor_get_duration_mode  (HildonTimeEditor * editor);

void       hildon_time_editor_set_duration_min   (HildonTimeEditor * editor, guint    duration_min);
guint      hildon_time_editor_get_duration_min   (HildonTimeEditor * editor);

void       hildon_time_editor_set_duration_max   (HildonTimeEditor * editor, guint    duration_max);
guint      hildon_time_editor_get_duration_max   (HildonTimeEditor * editor);


G_END_DECLS
#endif /* __HILDON_TIME_EDITOR_H__ */
