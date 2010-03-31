/* This file is part of herzi's playground
 *
 * Copyright (C) 2010  Sven Herzberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "gutachter-bar.h"

G_DEFINE_TYPE (GutachterBar, gutachter_bar, GTK_TYPE_BIN);

static void
gutachter_bar_init (GutachterBar* self)
{
  gtk_container_set_border_width (GTK_CONTAINER (self), 6);
}

static void
size_allocate (GtkWidget    * widget,
               GtkAllocation* allocation)
{
  GtkWidget* child = gtk_bin_get_child (GTK_BIN (widget));
  guint      border = gtk_container_get_border_width (GTK_CONTAINER (widget));

  gtk_widget_set_allocation (widget, allocation);

  if ((allocation->width >= 0) && ((guint)allocation->width) > 2 * border)
    {
      allocation->x += border;
      allocation->width -= 2 * border;
    }
  if ((allocation->height >= 0) && ((guint)allocation->height) > 2 * border)
    {
      allocation->y += border;
      allocation->height -= 2 * border;
    }

  if (child)
    {
      gtk_widget_size_allocate (child, allocation);
    }
}

static void
size_request (GtkWidget     * widget,
              GtkRequisition* requisition)
{
  GtkWidget* child = gtk_bin_get_child (GTK_BIN (widget));

  requisition->width = 1;
  requisition->height = 1;

  if (child)
    {
      gtk_widget_size_request (child, requisition);

      requisition->width += 2 * gtk_container_get_border_width (GTK_CONTAINER (widget));
      requisition->height += 2 * gtk_container_get_border_width (GTK_CONTAINER (widget));
    }
}

static void
gutachter_bar_class_init (GutachterBarClass* self_class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);

  widget_class->size_allocate = size_allocate;
  widget_class->size_request  = size_request;
}

GtkWidget*
gutachter_bar_new (void)
{
  return g_object_new (GUTACHTER_TYPE_BAR,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
