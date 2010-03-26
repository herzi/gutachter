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

#include "gt-window.h"

#include <gtk-test.h>

struct _GtkTestWindowPrivate
{
  GtkToolItem* execute_button;
  GtkToolItem* open_button;
  GtkWidget  * toolbar;
  GtkWidget  * widget;
};

#define PRIV(i) (((GtkTestWindow*)(i))->_private)

G_DEFINE_TYPE (GtkTestWindow, gtk_test_window, GTK_TYPE_WINDOW);

static void
gtk_test_window_init (GtkTestWindow* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GTK_TEST_TYPE_WINDOW, GtkTestWindowPrivate);
  PRIV (self)->toolbar = gtk_toolbar_new ();
  PRIV (self)->widget = gtk_test_widget_new ();
  PRIV (self)->open_button = gtk_tool_button_new_from_stock (GTK_STOCK_OPEN);
  PRIV (self)->execute_button = gtk_tool_button_new_from_stock (GTK_STOCK_EXECUTE);

  gtk_window_set_default_size (GTK_WINDOW (self), 300, 400);

  gtk_toolbar_insert (GTK_TOOLBAR (PRIV (self)->toolbar), PRIV (self)->open_button, -1);
  gtk_toolbar_insert (GTK_TOOLBAR (PRIV (self)->toolbar), gtk_separator_tool_item_new (), -1);
  gtk_toolbar_insert (GTK_TOOLBAR (PRIV (self)->toolbar), PRIV (self)->execute_button, -1);
}

static void
gtk_test_window_class_init (GtkTestWindowClass* self_class)
{
  g_type_class_add_private (self_class, sizeof (GtkTestWindowPrivate));
}

GtkWidget*
gtk_test_window_get_exec (GtkTestWindow* self)
{
  g_return_val_if_fail (GTK_TEST_IS_WINDOW (self), NULL);

  return GTK_WIDGET (PRIV (self)->execute_button);
}

GtkWidget*
gtk_test_window_get_open (GtkTestWindow* self)
{
  g_return_val_if_fail (GTK_TEST_IS_WINDOW (self), NULL);

  return GTK_WIDGET (PRIV (self)->open_button);
}

GtkWidget*
gtk_test_window_get_toolbar (GtkTestWindow* self)
{
  g_return_val_if_fail (GTK_TEST_IS_WINDOW (self), NULL);

  return PRIV (self)->toolbar;
}

GtkWidget*
gtk_test_window_get_widget (GtkTestWindow* self)
{
  g_return_val_if_fail (GTK_TEST_IS_WINDOW (self), NULL);

  return PRIV (self)->widget;
}

GtkWidget*
gtk_test_window_new (void)
{
  return g_object_new (GTK_TEST_TYPE_WINDOW,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
