/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
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
 * SECTION:hildon-window-stack
 * @short_description: Object representing a stack of windows in the Hildon framework
 * @see_also: #HildonStackableWindow
 *
 * The #HildonWindowStack is an object used to represent a stack of
 * windows in the Hildon framework.
 *
 * Stacks contain all #HildonStackableWindow<!-- -->s that are being
 * shown. The user can only interact with the topmost window from each
 * stack (as it covers all the others), but all of them are mapped and
 * visible from the Gtk point of view.
 *
 * Each window can only be in one stack at a time. All stacked windows
 * are visible and all visible windows are stacked.
 *
 * Each application has a default stack, and windows are automatically
 * added to it when they are shown with gtk_widget_show().
 *
 * Additional stacks can be created at any time using
 * hildon_window_stack_new(). To add a window to a specific stack, use
 * hildon_window_stack_push_1() (remember that, for the default stack,
 * gtk_widget_show() can be used instead).
 *
 * To remove a window from a stack use hildon_window_stack_pop_1(), or
 * simply gtk_widget_hide().
 *
 * For more complex layout changes, applications can push and/or pop
 * several windows at the same time in a single step. See
 * hildon_window_stack_push(), hildon_window_stack_pop() and
 * hildon_window_stack_pop_and_push() for more details.
 */

#include                                        "hildon-window-stack.h"
#include                                        "hildon-window-stack-private.h"
#include                                        "hildon-stackable-window-private.h"

struct                                          _HildonWindowStackPrivate
{
    GList *list;
    GtkWindowGroup *group;
};

#define                                         HILDON_WINDOW_STACK_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj),\
                                                HILDON_TYPE_WINDOW_STACK, HildonWindowStackPrivate))

G_DEFINE_TYPE (HildonWindowStack, hildon_window_stack, G_TYPE_OBJECT);

/**
 * hildon_window_stack_get_default:
 *
 * Returns the default window stack. This stack always exists and
 * doesn't need to be created by the application.
 *
 * Return value: the default #HildonWindowStack
 **/
HildonWindowStack *
hildon_window_stack_get_default                 (void)
{
    static HildonWindowStack *stack = NULL;
    if (G_UNLIKELY (stack == NULL)) {
        stack = g_object_new (HILDON_TYPE_WINDOW_STACK, NULL);
        stack->priv->group = gtk_window_get_group (NULL);
    }
    return stack;
}

/**
 * hildon_window_stack_new:
 *
 * Creates a new #HildonWindowStack. The stack is initially empty.
 *
 * Return value: a new #HildonWindowStack
 **/
HildonWindowStack *
hildon_window_stack_new                         (void)
{
    HildonWindowStack *stack = g_object_new (HILDON_TYPE_WINDOW_STACK, NULL);
    stack->priv->group = gtk_window_group_new ();
    return stack;
}

/**
 * hildon_window_stack_size:
 * @stack: A #HildonWindowStack
 *
 * Returns the number of windows in @stack
 *
 * Return value: Number of windows in @stack
 **/
gint
hildon_window_stack_size                        (HildonWindowStack *stack)
{
    g_return_val_if_fail (HILDON_IS_WINDOW_STACK (stack), 0);

    return g_list_length (stack->priv->list);
}

/* Remove a window from its stack, no matter its position */
void G_GNUC_INTERNAL
hildon_window_stack_remove                      (HildonStackableWindow *win)
{
    HildonWindowStack *stack = hildon_stackable_window_get_stack (win);

    /* If the window is stacked */
    if (stack) {
        hildon_stackable_window_set_stack (win, NULL, -1);
        stack->priv->list = g_list_remove (stack->priv->list, win);
        gtk_window_set_transient_for (GTK_WINDOW (win), NULL);
    }
}

/**
 * hildon_window_stack_peek:
 * @stack: A %HildonWindowStack
 *
 * Returns the window on top of @stack. The stack is never modified.
 *
 * Return value: the window on top of the stack, or %NULL if the stack
 * is empty.
 **/
GtkWidget *
hildon_window_stack_peek                        (HildonWindowStack *stack)
{
    GtkWidget *win = NULL;

    g_return_val_if_fail (HILDON_IS_WINDOW_STACK (stack), NULL);

    if (stack->priv->list != NULL) {
        win = GTK_WIDGET (stack->priv->list->data);
    }

    return win;
}

static gboolean
_hildon_window_stack_do_push                    (HildonWindowStack     *stack,
                                                 HildonStackableWindow *win)
{
    HildonWindowStack *current_stack;

    g_return_val_if_fail (HILDON_IS_WINDOW_STACK (stack), FALSE);
    g_return_val_if_fail (HILDON_IS_STACKABLE_WINDOW (win), FALSE);

    current_stack = hildon_stackable_window_get_stack (win);

    if (current_stack == NULL) {
        GtkWidget *parent = hildon_window_stack_peek (stack);

        /* Push the window */
        hildon_stackable_window_set_stack (win, stack, g_list_length (stack->priv->list));
        stack->priv->list = g_list_prepend (stack->priv->list, win);

        /* Make the window part of the same group as its parent */
        if (parent) {
            gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW (parent));
        } else {
            gtk_window_group_add_window (stack->priv->group, GTK_WINDOW (win));
        }

        return TRUE;
    } else {
        g_warning ("Trying to push a window that is already on a stack");
        return FALSE;
    }
}

static GtkWidget *
_hildon_window_stack_do_pop                     (HildonWindowStack *stack)
{
    GtkWidget *win = hildon_window_stack_peek (stack);

    if (win)
        hildon_window_stack_remove (HILDON_STACKABLE_WINDOW (win));

    return win;
}

/**
 * hildon_window_stack_push_1:
 * @stack: A %HildonWindowStack
 * @win: A %HildonStackableWindow
 *
 * Adds @win to the top of @stack, and shows it. The window must not
 * be already stacked.
 **/
void
hildon_window_stack_push_1                      (HildonWindowStack     *stack,
                                                 HildonStackableWindow *win)
{
    if (_hildon_window_stack_do_push (stack, win))
        gtk_widget_show (GTK_WIDGET (win));
}

/**
 * hildon_window_stack_pop_1:
 * @stack: A %HildonWindowStack
 *
 * Removes the window on top of @stack, and hides it. If the stack is
 * empty nothing happens.
 *
 * Return value: the window on top of the stack, or %NULL if the stack
 * is empty.
 **/
GtkWidget *
hildon_window_stack_pop_1                       (HildonWindowStack *stack)
{
    GtkWidget *win = _hildon_window_stack_do_pop (stack);
    if (win)
        gtk_widget_hide (win);
    return win;
}

/**
 * hildon_window_stack_push_list:
 * @stack: A %HildonWindowStack
 * @list: A list of %HildonStackableWindow<!-- -->s to push
 *
 * Pushes all windows in @list to the top of @stack, and shows
 * them. Everything is done in a single transition, so the user will
 * only see the last window in @list during this operation. None of
 * the windows must be already stacked.
 **/
void
hildon_window_stack_push_list                   (HildonWindowStack *stack,
                                                 GList             *list)
{
    HildonStackableWindow *win;
    GList *l;
    GList *pushed = NULL;

    g_return_if_fail (HILDON_IS_WINDOW_STACK (stack));

    /* Stack all windows */
    for (l = list; l != NULL; l = g_list_next (l)) {
        win = HILDON_STACKABLE_WINDOW (l->data);
        if (win) {
            _hildon_window_stack_do_push (stack, win);
            pushed = g_list_prepend (pushed, win);
        } else {
            g_warning ("Trying to stack a non-stackable window!");
        }
    }

    /* Show windows in reverse order (topmost first) */
    g_list_foreach (pushed, (GFunc) gtk_widget_show, NULL);

    g_list_free (pushed);
}

/**
 * hildon_window_stack_push:
 * @stack: A %HildonWindowStack
 * @win1: The first window to push
 * @Varargs: A %NULL-terminated list of additional #HildonStackableWindow<!-- -->s to push.
 *
 * Pushes all windows to the top of @stack, and shows them. Everything
 * is done in a single transition, so the user will only see the last
 * window. None of the windows must be already stacked.
 **/
void
hildon_window_stack_push                        (HildonWindowStack     *stack,
                                                 HildonStackableWindow *win1,
                                                 ...)
{
    HildonStackableWindow *win;
    GList *list = NULL;
    va_list args;

    va_start (args, win1);
    win = va_arg (args, HildonStackableWindow *);

    while (win != NULL) {
        list = g_list_prepend (list, win);
        win = va_arg (args, HildonStackableWindow *);
    }

    va_end (args);

    hildon_window_stack_push_list (stack, list);
    g_list_free (list);
}

/**
 * hildon_window_stack_pop:
 * @stack: A %HildonWindowStack
 * @nwindows: Number of windows to pop
 * @popped_windows: if non-%NULL, the list of popped windows is stored here
 *
 * Pops @nwindows windows from @stack, and hides them. Everything is
 * done in a single transition, so the user will not see any of the
 * windows being popped in this operation.
 *
 * If @popped_windows is not %NULL, the list of popped windows is
 * stored there (ordered bottom-up). That list must be freed by the
 * user.
 **/
void
hildon_window_stack_pop                         (HildonWindowStack  *stack,
                                                 gint                nwindows,
                                                 GList             **popped_windows)
{
    gint i;
    GList *popped = NULL;

    g_return_if_fail (HILDON_IS_WINDOW_STACK (stack));
    g_return_if_fail (nwindows > 0);
    g_return_if_fail (g_list_length (stack->priv->list) >= nwindows);

    /* Pop windows */
    for (i = 0; i < nwindows; i++) {
        GtkWidget *win = _hildon_window_stack_do_pop (stack);
        popped = g_list_prepend (popped, win);
    }

    /* Hide windows in reverse order (topmost last) */
    g_list_foreach (popped, (GFunc) gtk_widget_hide, NULL);

    if (popped_windows) {
        *popped_windows = popped;
    } else {
        g_list_free (popped);
    }
}

/**
 * hildon_window_stack_pop_and_push_list:
 * @stack: A %HildonWindowStack
 * @nwindows: Number of windows to pop.
 * @popped_windows: if non-%NULL, the list of popped windows is stored here
 * @list: A list of %HildonStackableWindow<!-- -->s to push
 *
 * Pops @nwindows windows from @stack (and hides them), then pushes
 * all windows in @list (and shows them). Everything is done in a
 * single transition, so the user will only see the last window from
 * @list. None of the pushed windows must be already stacked.
 *
 * If @popped_windows is not %NULL, the list of popped windows is
 * stored there (ordered bottom-up). That list must be freed by the
 * user.
 **/
void
hildon_window_stack_pop_and_push_list           (HildonWindowStack  *stack,
                                                 gint                nwindows,
                                                 GList             **popped_windows,
                                                 GList              *list)
{
    gint i;
    GList *l;
    GList *popped = NULL;
    GList *pushed = NULL;

    g_return_if_fail (HILDON_IS_WINDOW_STACK (stack));
    g_return_if_fail (nwindows > 0);
    g_return_if_fail (g_list_length (stack->priv->list) >= nwindows);

    /* Pop windows */
    for (i = 0; i < nwindows; i++) {
        GtkWidget *win = _hildon_window_stack_do_pop (stack);
        popped = g_list_prepend (popped, win);
    }

    /* Push windows */
    for (l = list; l != NULL; l = g_list_next (l)) {
        HildonStackableWindow *win = HILDON_STACKABLE_WINDOW (l->data);
        if (win) {
            _hildon_window_stack_do_push (stack, win);
            pushed = g_list_prepend (pushed, win);
        } else {
            g_warning ("Trying to stack a non-stackable window!");
        }
    }

    /* Show windows in reverse order (topmost first) */
    g_list_foreach (pushed, (GFunc) gtk_widget_show, NULL);

    /* Hide windows in reverse order (topmost last) */
    g_list_foreach (popped, (GFunc) gtk_widget_hide, NULL);

    g_list_free (pushed);
    if (popped_windows) {
        *popped_windows = popped;
    } else {
        g_list_free (popped);
    }
}

/**
 * hildon_window_stack_pop_and_push:
 * @stack: A %HildonWindowStack
 * @nwindows: Number of windows to pop.
 * @popped_windows: if non-%NULL, the list of popped windows is stored here
 * @win1: The first window to push
 * @Varargs: A %NULL-terminated list of additional #HildonStackableWindow<!-- -->s to push.
 *
 * Pops @nwindows windows from @stack (and hides them), then pushes
 * all passed windows (and shows them). Everything is done in a single
 * transition, so the user will only see the last pushed window. None
 * of the pushed windows must be already stacked.
 *
 * If @popped_windows is not %NULL, the list of popped windows is
 * stored there (ordered bottom-up). That list must be freed by the
 * user.
 **/
void
hildon_window_stack_pop_and_push                (HildonWindowStack      *stack,
                                                 gint                    nwindows,
                                                 GList                 **popped_windows,
                                                 HildonStackableWindow  *win1,
                                                 ...)
{
    HildonStackableWindow *win;
    GList *list = NULL;
    va_list args;

    va_start (args, win1);
    win = va_arg (args, HildonStackableWindow *);

    while (win != NULL) {
        list = g_list_prepend (list, win);
        win = va_arg (args, HildonStackableWindow *);
    }

    va_end (args);

    hildon_window_stack_pop_and_push_list (stack, nwindows, popped_windows, list);
    g_list_free (list);
}

static void
hildon_window_stack_finalize (GObject *object)
{
    HildonWindowStack *stack = HILDON_WINDOW_STACK (object);

    if (stack->priv->list)
        hildon_window_stack_pop (stack, hildon_window_stack_size (stack), NULL);

    if (stack->priv->group)
        g_object_unref (stack->priv->group);

    G_OBJECT_CLASS (hildon_window_stack_parent_class)->finalize (object);
}

static void
hildon_window_stack_class_init (HildonWindowStackClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *)klass;

    gobject_class->finalize = hildon_window_stack_finalize;

    g_type_class_add_private (klass, sizeof (HildonWindowStackPrivate));
}

static void
hildon_window_stack_init (HildonWindowStack *self)
{
    HildonWindowStackPrivate *priv;

    priv = self->priv = HILDON_WINDOW_STACK_GET_PRIVATE (self);

    priv->list = NULL;
    priv->group = NULL;
}