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
 
#ifndef __HILDON_CAPTION_H__
#define __HILDON_CAPTION_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkeventbox.h>
#include <gtk/gtksizegroup.h>

G_BEGIN_DECLS


#define HILDON_TYPE_CAPTION ( hildon_caption_get_type() )
#define HILDON_CAPTION(obj) \
    (GTK_CHECK_CAST (obj, HILDON_TYPE_CAPTION, HildonCaption))
#define HILDON_CAPTION_CLASS(klass) \
    (GTK_CHECK_CLASS_CAST ((klass),\
     HILDON_TYPE_CAPTION, HildonCaptionClass))
#define HILDON_IS_CAPTION(obj) (GTK_CHECK_TYPE (obj, HILDON_TYPE_CAPTION))
#define HILDON_IS_CAPTION_CLASS(klass) \
    (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_CAPTION))

    
/**
 * HildonCaptionStatus:
 * @HILDON_CAPTION_OPTIONAL: Optional.
 * @HILDON_CAPTION_MANDATORY: Mandatory.
 *
 * Keys to set the #HildonCaption to be optional or mandatory.
 */
typedef enum {
    HILDON_CAPTION_OPTIONAL = 0,
    HILDON_CAPTION_MANDATORY
} HildonCaptionStatus;

#define HILDON_TYPE_CAPTION_STATUS (hildon_caption_status_get_type ())

GType hildon_caption_status_get_type (void) G_GNUC_CONST;

/**
 * HildonCaption:
 *
 * Contains only private data.
 */
typedef struct _HildonCaption HildonCaption;
typedef struct _HildonCaptionClass HildonCaptionClass;


struct _HildonCaption
{
  GtkEventBox event_box;
};


struct _HildonCaptionClass
{
  GtkEventBoxClass parent_class;
  void (*activate) (GtkWidget *widget);
};


GType hildon_caption_get_type (void) G_GNUC_CONST;

GtkWidget *hildon_caption_new( GtkSizeGroup *group, const gchar *value,
                               GtkWidget *control, GtkWidget *icon,
                               HildonCaptionStatus flag );
#ifndef HILDON_DISABLE_DEPRECATED
GtkSizeGroup *hildon_caption_get_sizegroup( const HildonCaption *caption );

void hildon_caption_set_sizegroup( const HildonCaption *caption,
                                   GtkSizeGroup *new_group );
#endif

gboolean hildon_caption_is_mandatory( const HildonCaption *caption );

void hildon_caption_set_status( HildonCaption *caption,
                               HildonCaptionStatus flag );

HildonCaptionStatus hildon_caption_get_status( const HildonCaption *caption );

void hildon_caption_set_icon_image( HildonCaption *caption, GtkWidget *icon );

GtkWidget *hildon_caption_get_icon_image(const HildonCaption *caption);

void hildon_caption_set_label( HildonCaption *caption, const gchar *label );

gchar *hildon_caption_get_label( const HildonCaption *caption );

void hildon_caption_set_separator( HildonCaption *caption, 
                                    const gchar *separator );

gchar *hildon_caption_get_separator( const HildonCaption *caption );

void hildon_caption_set_label_alignment(HildonCaption *caption, 
                                        gfloat alignment);
gfloat hildon_caption_get_label_alignment(HildonCaption *caption);

#ifndef HILDON_DISABLE_DEPRECATED
GtkWidget *hildon_caption_get_control( const HildonCaption *caption );

void hildon_caption_set_control( HildonCaption *caption, GtkWidget *control );
#endif

void hildon_caption_set_child_expand( HildonCaption *caption, gboolean expand );
gboolean hildon_caption_get_child_expand( const HildonCaption *caption );

G_END_DECLS
#endif /* __HILDON_CAPTION_H__ */