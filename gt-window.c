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
  GtkWidget  * box;
  GtkToolItem* execute_button;
  GtkToolItem* open_button;
  GtkWidget  * toolbar;
  GtkWidget  * widget;
};

enum
{
  PROP_0,
  PROP_TEST_SUITE
};

#define PRIV(i) (((GtkTestWindow*)(i))->_private)

static void implement_gtk_test_runner (GtkTestRunnerIface* iface);

G_DEFINE_TYPE_WITH_CODE (GtkTestWindow, gtk_test_window, GTK_TYPE_WINDOW,
                         G_IMPLEMENT_INTERFACE (GTK_TEST_TYPE_RUNNER, implement_gtk_test_runner));

static void
forward_notify (GObject   * object G_GNUC_UNUSED,
                GParamSpec* pspec,
                gpointer    user_data)
{
  g_object_notify (user_data, pspec->name);
}

static void
gtk_test_window_init (GtkTestWindow* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GTK_TEST_TYPE_WINDOW, GtkTestWindowPrivate);
  PRIV (self)->toolbar = gtk_toolbar_new ();
  PRIV (self)->widget = gtk_test_widget_new ();
  PRIV (self)->open_button = gtk_tool_button_new_from_stock (GTK_STOCK_OPEN);
  PRIV (self)->execute_button = gtk_tool_button_new_from_stock (GTK_STOCK_EXECUTE);
  PRIV (self)->box = gtk_vbox_new (FALSE, 0);

  gtk_window_set_default_size (GTK_WINDOW (self), 300, 400);

  gtk_toolbar_insert (GTK_TOOLBAR (PRIV (self)->toolbar), PRIV (self)->open_button, -1);
  gtk_toolbar_insert (GTK_TOOLBAR (PRIV (self)->toolbar), gtk_separator_tool_item_new (), -1);
  gtk_toolbar_insert (GTK_TOOLBAR (PRIV (self)->toolbar), PRIV (self)->execute_button, -1);

  gtk_widget_show_all (PRIV (self)->toolbar);
  gtk_box_pack_start (GTK_BOX (PRIV (self)->box), PRIV (self)->toolbar, FALSE, FALSE, 0);
  gtk_widget_show (PRIV (self)->widget);
  gtk_container_add (GTK_CONTAINER (PRIV (self)->box), PRIV (self)->widget);
  gtk_widget_show (PRIV (self)->box);
  gtk_container_add (GTK_CONTAINER (self), PRIV (self)->box);

  g_signal_connect (PRIV (self)->widget, "notify::test-suite",
                    G_CALLBACK (forward_notify), self);
}

static void
get_property (GObject   * object,
              guint       prop_id,
              GValue    * value,
              GParamSpec* pspec)
{
  switch (prop_id)
    {
    case PROP_TEST_SUITE:
      g_object_get_property (G_OBJECT (PRIV (object)->widget), pspec->name, value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
destroy (GtkObject* object)
{
  PRIV (object)->execute_button = NULL;

  GTK_OBJECT_CLASS (gtk_test_window_parent_class)->destroy (object);
}

static void
gtk_test_window_class_init (GtkTestWindowClass* self_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS (self_class);
  GtkObjectClass* gtk_object_class = GTK_OBJECT_CLASS (self_class);

  object_class->get_property = get_property;

  g_object_class_override_property (object_class, PROP_TEST_SUITE, "test-suite");

  gtk_object_class->destroy = destroy;

  g_type_class_add_private (self_class, sizeof (GtkTestWindowPrivate));
}

static GFile*
get_file (GtkTestRunner* runner)
{
  return gtk_test_runner_get_file (GTK_TEST_RUNNER (PRIV (runner)->widget));
}

static GtkTestSuite*
get_suite (GtkTestRunner* runner)
{
  return gtk_test_runner_get_suite (GTK_TEST_RUNNER (PRIV (runner)->widget));
}

static void
set_file (GtkTestRunner* runner,
          GFile        * file)
{
  gtk_test_runner_set_file (GTK_TEST_RUNNER (PRIV (runner)->widget), file);
}

static void
implement_gtk_test_runner (GtkTestRunnerIface* iface)
{
  iface->get_file  = get_file;
  iface->get_suite = get_suite;
  iface->set_file  = set_file;
}

GtkWidget*
gtk_test_window_get_box (GtkTestWindow* self)
{
  g_return_val_if_fail (GTK_TEST_IS_WINDOW (self), NULL);

  return PRIV (self)->box;
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
