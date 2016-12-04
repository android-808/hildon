/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Rodrigo Novo <rodrigo.novo@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
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

/**
 * SECTION:hildon-hvolumebar
 * @short_description: A widget that displays a horizontal volume bar.
 * @see_also: #HildonVVolumebar, #HildonVolumebar
 * 
 * The #HildonHVolumebar widget displays a horizontal volume bar that allows
 * increasing or decreasing volume within a pre-defined range, and includes 
 * a mute icon which users can click to mute the sound.
 *
 * <note>
 *   <para>
 * #HildonHVolumeBar has been deprecated since Hildon 2.2
 * See <link linkend="hildon-migrating-volume-bar">Migrating Volume Bars</link>
 * section to know how to migrate this deprecated widget.
 *   </para>
 * </note>
 *
 * <example>
 * <programlisting>
 * GtkWidget *volbar = hildon_hvolumebar_new ();
 * g_signal_connect (G_OBJECT(volbar), "mute_toggled", G_CALLBACK(mute_toggle), NULL);
 * g_signal_connect (G_OBJECT(volbar), "level_changed", G_CALLBACK(level_change), NULL);
 * </programlisting>
 * </example>
 *
 */

#undef                                          HILDON_DISABLE_DEPRECATED

#include                                        "hildon-hvolumebar.h"
#include                                        "hildon-volumebar.h"
#include                                        "hildon-volumebar-range.h"
#include                                        "hildon-volumebar-private.h"

/* Defines for normal version of HVolumebar */
/* Toggle button */

#define                                         DEFAULT_TBUTTON_WIDTH 60

#define                                         DEFAULT_TBUTTON_HEIGHT 60

/* Volume bar */
#define                                         MINIMUM_BAR_WIDTH 147

#define                                         DEFAULT_BAR_HEIGHT 60

#define                                         DEFAULT_ENDING_SIZE 20

/* Gap to leave for mute button */
#define                                         VERTICAL_MUTE_GAP 0

#define                                         HORIZONTAL_MUTE_GAP 0

/* Sizes inside a toolbar */
/* Toggle button */

#define                                         TOOL_DEFAULT_TBUTTON_WIDTH 26

#define                                         TOOL_DEFAULT_TBUTTON_HEIGHT 26

/* Volumebar */

#define                                         TOOL_MINIMUM_BAR_WIDTH 121

#define                                         TOOL_DEFAULT_BAR_HEIGHT 40

#define                                         TOOL_DEFAULT_ENDING_SIZE 0

#define                                         TOOL_VERTICAL_MUTE_GAP \
                                                ((TOOL_DEFAULT_BAR_HEIGHT - TOOL_DEFAULT_TBUTTON_HEIGHT) / 2)

static HildonVolumebarClass*                    parent_class;

static void 
hildon_hvolumebar_class_init                    (HildonHVolumebarClass *klass);

static void 
hildon_hvolumebar_init                          (HildonHVolumebar *hvolumebar);

static gboolean
hildon_hvolumebar_draw                          (GtkWidget *widget,
                                                 cairo_t *cr);
static void
hildon_hvolumebar_get_preferred_width           (GtkWidget *widget,
                                                 gint      *minimal_width,
                                                 gint      *natural_width);
static void
hildon_hvolumebar_get_preferred_height          (GtkWidget *widget,
                                                 gint      *minimal_height,
                                                 gint      *natural_height);
static void 
hildon_hvolumebar_size_request                  (GtkWidget *widget,
                                                 GtkRequisition *requisition);
static void 
hildon_hvolumebar_size_allocate                 (GtkWidget *widget,
                                                 GtkAllocation *allocation);
static void 
hildon_hvolumebar_map                           (GtkWidget *widget);

/**
 * hildon_hvolumebar_get_type:
 *
 * Returns GType for HildonHVolumebar.
 *
 * Returns: HildonHVolumebar type
 */
GType G_GNUC_CONST
hildon_hvolumebar_get_type                      (void)
{
    static GType type = 0;

    if (!type) {
        static const GTypeInfo info = {
            sizeof (HildonHVolumebarClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_hvolumebar_class_init,     /* class_init */
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof (HildonHVolumebar),
            0,
            (GInstanceInitFunc) hildon_hvolumebar_init,
        };
        type = g_type_register_static (HILDON_TYPE_VOLUMEBAR,
                "HildonHVolumebar", &info, 0);
    }
    return type;
}

static void
hildon_hvolumebar_class_init                    (HildonHVolumebarClass *klass)
{
    GtkWidgetClass *volumebar_class = GTK_WIDGET_CLASS (klass);

    parent_class = g_type_class_peek_parent (klass);

    volumebar_class->get_preferred_width  = hildon_hvolumebar_get_preferred_width;
    volumebar_class->get_preferred_height = hildon_hvolumebar_get_preferred_height;
    volumebar_class->size_allocate        = hildon_hvolumebar_size_allocate;
    volumebar_class->map                  = hildon_hvolumebar_map;
    volumebar_class->draw                 = hildon_hvolumebar_draw;
}

static void 
hildon_hvolumebar_init                          (HildonHVolumebar *hvolumebar)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE (hvolumebar);

    priv->volumebar =
        HILDON_VOLUMEBAR_RANGE(hildon_volumebar_range_new
                (GTK_ORIENTATION_HORIZONTAL));

    gtk_widget_set_parent (GTK_WIDGET (priv->tbutton), GTK_WIDGET (hvolumebar));
    gtk_widget_set_parent (GTK_WIDGET (priv->volumebar), GTK_WIDGET (hvolumebar));

    gtk_scale_set_draw_value (GTK_SCALE (priv->volumebar), FALSE);

    /* Signals */
    g_signal_connect_swapped (G_OBJECT (priv->volumebar), "value-changed",
            G_CALLBACK(hildon_volumebar_level_change),
            hvolumebar);

    g_signal_connect_swapped (priv->tbutton, "toggled",
            G_CALLBACK (hildon_volumebar_mute_toggled), hvolumebar);

    gtk_widget_show (GTK_WIDGET (priv->volumebar));
}

/**
 * hildon_hvolumebar_new:
 *
 * Creates a new #HildonHVolumebar widget.
 *
 * Returns: a new #HildonHVolumebar
 */
GtkWidget*
hildon_hvolumebar_new                           (void)
{
    return GTK_WIDGET (g_object_new(HILDON_TYPE_HVOLUMEBAR, NULL));
}

static void 
hildon_hvolumebar_map                           (GtkWidget* widget)
{
    HildonVolumebarPrivate *priv;
    GtkWidget *parent;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(widget);
    g_assert (priv);

    parent = gtk_widget_get_ancestor (GTK_WIDGET (widget), GTK_TYPE_TOOLBAR);

    /* Check if the volumebar is in a toolbar */
    if (parent)
        priv->is_toolbar = TRUE;

    GTK_WIDGET_CLASS (parent_class)->map (widget);
}

static gboolean 
hildon_hvolumebar_draw                          (GtkWidget *widget,
                                                 cairo_t *cr)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(HILDON_VOLUMEBAR(widget));
    g_assert (priv);

    if (gtk_widget_is_drawable (widget)) {

        /* Paint background */
        gint width, height;
        GtkStyleContext *context;

        width = gtk_widget_get_allocated_width (widget);
        height = gtk_widget_get_allocated_height (widget);
        context = gtk_widget_get_style_context (widget);

        gtk_render_background (context, cr, 0, 0, width, height);
        gtk_render_frame (context, cr, 0, 0, width, height);


/*        gtk_paint_box (widget->style, cr,
                gtk_widget_get_state (priv->volumebar), GTK_SHADOW_OUT,
                widget, "background",
                allocation.x,
                allocation.y,
                allocation.width,
                allocation.height);*/

        /* The contents of the widget can paint themselves */
        /* FIXME Not sure if this is even needed here */
        (*GTK_WIDGET_CLASS(parent_class)->draw) (widget, cr);
    }

    return FALSE;
}

static void
hildon_hvolumebar_get_preferred_width (GtkWidget *widget,
                               gint      *minimal_width,
                               gint      *natural_width)
{
  GtkRequisition requisition;

  hildon_hvolumebar_size_request (widget, &requisition);

  *minimal_width = *natural_width = requisition.width;
}

static void
hildon_hvolumebar_get_preferred_height (GtkWidget *widget,
                                gint      *minimal_height,
                                gint      *natural_height)
{
  GtkRequisition requisition;

  hildon_hvolumebar_size_request (widget, &requisition);

  *minimal_height = *natural_height = requisition.height;
}

static void
hildon_hvolumebar_size_request                  (GtkWidget * widget,
                                                 GtkRequisition * requisition)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(HILDON_VOLUMEBAR(widget));
    g_assert (priv);

    /* Volumebar has different dimensions in toolbar */
    requisition->width = (priv->is_toolbar
            ? TOOL_MINIMUM_BAR_WIDTH
            : MINIMUM_BAR_WIDTH);

    requisition->height = (priv->is_toolbar
            ? TOOL_DEFAULT_BAR_HEIGHT
            : DEFAULT_BAR_HEIGHT);
}

static void
hildon_hvolumebar_size_allocate                 (GtkWidget * widget,
                                                 GtkAllocation * allocation)
{
    HildonVolumebarPrivate *priv;
    GtkAllocation button_allocation, range_allocation;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(widget);
    g_assert (priv);

    button_allocation.x = 0;
    button_allocation.width = 0;

    /* Center the widget vertically */
    if (priv->is_toolbar && allocation->height > TOOL_DEFAULT_BAR_HEIGHT) {
        allocation->y += (allocation->height - TOOL_DEFAULT_BAR_HEIGHT) / 2;
        allocation->height = TOOL_DEFAULT_BAR_HEIGHT;
    }

    if (!priv->is_toolbar && allocation->height > DEFAULT_BAR_HEIGHT) {
        allocation->y += (allocation->height - DEFAULT_BAR_HEIGHT) / 2;
        allocation->height = DEFAULT_BAR_HEIGHT;
    }

    GTK_WIDGET_CLASS (parent_class)->size_allocate (widget, allocation);

    if (priv->tbutton && gtk_widget_is_visible (GTK_WIDGET (priv->tbutton))) {

        /* Allocate space for the mute button */
        if (priv->is_toolbar) {
            button_allocation.x = allocation->x;
            button_allocation.y = allocation->y + TOOL_VERTICAL_MUTE_GAP;
            button_allocation.width = TOOL_DEFAULT_TBUTTON_WIDTH;
            button_allocation.height = TOOL_DEFAULT_TBUTTON_HEIGHT;
        } else {
            button_allocation.x = allocation->x + DEFAULT_ENDING_SIZE;
            button_allocation.y = allocation->y + VERTICAL_MUTE_GAP;
            button_allocation.width = DEFAULT_TBUTTON_WIDTH;
            button_allocation.height = DEFAULT_TBUTTON_HEIGHT;
        }
        gtk_widget_size_allocate (GTK_WIDGET (priv->tbutton),
                &button_allocation);
    }
    if (priv->volumebar && gtk_widget_is_visible (GTK_WIDGET (priv->volumebar))) {

        /* Allocate space for the slider */
        range_allocation.y = allocation->y;

        if (priv->tbutton && gtk_widget_is_visible (GTK_WIDGET (priv->tbutton)))
        {
            /* Leave room for the mute button */
            range_allocation.x = button_allocation.x
                + button_allocation.width
                + HORIZONTAL_MUTE_GAP;

            if (priv->is_toolbar) 
            {
                /* In toolbar with mute button */
                range_allocation.width = MAX(0,
                        allocation->width
                        - 2 * TOOL_DEFAULT_ENDING_SIZE
                        - TOOL_DEFAULT_TBUTTON_WIDTH
                        - HORIZONTAL_MUTE_GAP);

                range_allocation.height = TOOL_DEFAULT_BAR_HEIGHT;

            } 

            else 
            {
                /* Standalone with mute button */
                range_allocation.width = MAX(0,
                        allocation->width
                        - 2 * DEFAULT_ENDING_SIZE
                        - DEFAULT_TBUTTON_WIDTH
                        - HORIZONTAL_MUTE_GAP);

                range_allocation.height = DEFAULT_BAR_HEIGHT;
            }

        }

        else
        {
            if (priv->is_toolbar) 
            {
                /* In toolbar without mute button */
                range_allocation.x = allocation->x;

                range_allocation.width = MAX(0,
                        allocation->width
                        - 2 * TOOL_DEFAULT_ENDING_SIZE );

                range_allocation.height = TOOL_DEFAULT_BAR_HEIGHT;

            } 

            else 
            {
                /* Standalone without mute button */
                range_allocation.x = allocation->x + DEFAULT_ENDING_SIZE;

                range_allocation.width = MAX(0,
                        allocation->width
                        - 2 * DEFAULT_ENDING_SIZE );

                range_allocation.height = DEFAULT_BAR_HEIGHT;
            }
        }

        gtk_widget_size_allocate (GTK_WIDGET (priv->volumebar),
                &range_allocation);
    }
}

