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

#include "gutachter-window.h"

#include <stdlib.h>   /* exit() */
#include <unistd.h>   /* pipe() */
#include <gutachter.h>

#include <glib/gi18n.h>

/* FIXME: add a gallery image for the documentation */

struct _GtkTestWindowPrivate
{
  guint        auto_update : 1;
  GtkWidget  * box;
  GtkToolItem* execute_button;
  GtkToolItem* open_button;
  gulong       status_handler;
  GtkWidget  * toolbar;
  GtkWidget  * widget;
};

enum
{
  PROP_0,
  PROP_TEST_SUITE
};

#define PRIV(i) (((GtkTestWindow*)(i))->_private)

static void implement_gutachter_runner (GutachterRunnerIface* iface);

G_DEFINE_TYPE_WITH_CODE (GtkTestWindow, gtk_test_window, GTK_TYPE_WINDOW,
                         G_IMPLEMENT_INTERFACE (GUTACHTER_TYPE_RUNNER, implement_gutachter_runner));

static void
forward_notify (GObject   * object G_GNUC_UNUSED,
                GParamSpec* pspec,
                gpointer    user_data)
{
  GutachterSuite* suite;

  gtk_test_window_update_title (user_data);

  suite = gutachter_runner_get_suite (GUTACHTER_RUNNER (PRIV (user_data)->widget));
  if (suite)
    {
      gutachter_suite_load (suite);
    }

  g_object_notify (user_data, pspec->name);
}

static void
open_item_clicked (GtkButton* button G_GNUC_UNUSED,
                   GtkWindow* window)
{
  GtkFileFilter* filter;
  GtkWidget* dialog = gtk_file_chooser_dialog_new (_("Choose Unit Test"),
                                                   window,
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   GTK_STOCK_CLOSE, GTK_RESPONSE_REJECT,
                                                   GTK_STOCK_OPEN,  GTK_RESPONSE_ACCEPT,
                                                   NULL);

  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);

  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("Executable Programs"));
  gtk_file_filter_add_mime_type (filter, "application/x-executable");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog),
                               filter);

  if (GTK_RESPONSE_ACCEPT == gtk_dialog_run (GTK_DIALOG (dialog)))
    {
      GFile* file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
      gutachter_runner_set_file (GUTACHTER_RUNNER (window), file);
      g_object_unref (file);
    }

  gtk_widget_destroy (dialog);
}

static void
button_clicked_cb (GtkButton* button    G_GNUC_UNUSED,
                   gpointer   user_data)
{
  gutachter_suite_execute (gutachter_runner_get_suite (GUTACHTER_RUNNER (PRIV (user_data)->widget)));
}

static void
auto_update_toggled (GtkToggleToolButton* button,
                     gpointer             user_data)
{
  GtkTestWindow* self = user_data;
  gboolean       new_value = gtk_toggle_tool_button_get_active (button);

  if (PRIV (self)->auto_update == new_value)
    {
      return;
    }

  PRIV (self)->auto_update = new_value;

  if (PRIV (self)->auto_update)
    {
      GutachterSuite* suite = gutachter_runner_get_suite (GUTACHTER_RUNNER (PRIV (self)->widget));

      if (suite && GUTACHTER_SUITE_LOADED == gutachter_suite_get_status (suite))
        {
          gutachter_suite_execute (suite);
        }
    }
}

static void
gtk_test_window_init (GtkTestWindow* self)
{
  GtkToolItem* item;

  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GTK_TEST_TYPE_WINDOW, GtkTestWindowPrivate);
  PRIV (self)->toolbar = gtk_toolbar_new ();
  PRIV (self)->widget = gutachter_widget_new ();
  PRIV (self)->open_button = gtk_tool_button_new_from_stock (GTK_STOCK_OPEN);
  PRIV (self)->execute_button = gtk_tool_button_new_from_stock (GTK_STOCK_EXECUTE);
  PRIV (self)->box = gtk_vbox_new (FALSE, 0);

  gtk_window_set_default_size (GTK_WINDOW (self), 300, 400);
  gtk_window_set_icon_name (GTK_WINDOW (self), "gutachter");

  g_signal_connect (PRIV (self)->open_button, "clicked",
                    G_CALLBACK (open_item_clicked), self);

  g_signal_connect (PRIV (self)->execute_button, "clicked",
                    G_CALLBACK (button_clicked_cb), self);

  gtk_toolbar_insert (GTK_TOOLBAR (PRIV (self)->toolbar), PRIV (self)->open_button, -1);
  gtk_toolbar_insert (GTK_TOOLBAR (PRIV (self)->toolbar), gtk_separator_tool_item_new (), -1);
  gtk_toolbar_insert (GTK_TOOLBAR (PRIV (self)->toolbar), PRIV (self)->execute_button, -1);
  item = gtk_toggle_tool_button_new ();
  gtk_tool_button_set_label (GTK_TOOL_BUTTON (item), _("auto-update"));
  gtk_tool_item_set_is_important (item, TRUE);
  g_signal_connect (item, "toggled",
                    G_CALLBACK (auto_update_toggled), self);
  gtk_toolbar_insert (GTK_TOOLBAR (PRIV (self)->toolbar), item, -1);

  gtk_widget_show_all (PRIV (self)->toolbar);
  gtk_box_pack_start (GTK_BOX (PRIV (self)->box), PRIV (self)->toolbar, FALSE, FALSE, 0);
  gtk_widget_show (PRIV (self)->widget);
  gtk_container_add (GTK_CONTAINER (PRIV (self)->box), PRIV (self)->widget);
  gtk_widget_show (PRIV (self)->box);
  gtk_container_add (GTK_CONTAINER (self), PRIV (self)->box);

  g_signal_connect (PRIV (self)->widget, "notify::test-suite",
                    G_CALLBACK (forward_notify), self);

  gtk_test_window_update_title (self);
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
get_file (GutachterRunner* runner)
{
  return gutachter_runner_get_file (GUTACHTER_RUNNER (PRIV (runner)->widget));
}

static GutachterSuite*
get_suite (GutachterRunner* runner)
{
  return gutachter_runner_get_suite (GUTACHTER_RUNNER (PRIV (runner)->widget));
}

static void
status_changed_cb (GObject   * suite_object,
                   GParamSpec* pspec        G_GNUC_UNUSED,
                   gpointer    user_data)
{
  switch (gutachter_suite_get_status (GUTACHTER_SUITE (suite_object)))
    {
    case GUTACHTER_SUITE_LOADED:
      if (PRIV (user_data)->auto_update)
        {
          gutachter_suite_execute (GUTACHTER_SUITE (suite_object));
        }
    case GUTACHTER_SUITE_FINISHED:
      gtk_widget_set_sensitive (GTK_WIDGET (PRIV (user_data)->execute_button), TRUE);
      break;
    default:
      gtk_widget_set_sensitive (GTK_WIDGET (PRIV (user_data)->execute_button), FALSE);
      break;
    }
}

static void
set_file (GutachterRunner* runner,
          GFile          * file)
{
  GutachterSuite* suite;

  if (PRIV (runner)->status_handler)
    {
      g_signal_handler_disconnect (gutachter_runner_get_suite (GUTACHTER_RUNNER (PRIV (runner)->widget)),
                                   PRIV (runner)->status_handler);
      PRIV (runner)->status_handler = 0;
    }

  gutachter_runner_set_file (GUTACHTER_RUNNER (PRIV (runner)->widget), file);

  suite = gutachter_runner_get_suite (GUTACHTER_RUNNER (PRIV (runner)->widget));
  if (suite)
    {
      PRIV (runner)->status_handler = g_signal_connect (suite, "notify::status",
                                                        G_CALLBACK (status_changed_cb), runner);
      status_changed_cb (G_OBJECT (suite), NULL, runner);
    }
}

static void
implement_gutachter_runner (GutachterRunnerIface* iface)
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

void
gtk_test_window_update_title (GtkTestWindow* self)
{
  GFile* testcase;
  gchar* title = NULL;

  g_return_if_fail (GTK_TEST_IS_WINDOW (self));

  testcase = gutachter_runner_get_file (GUTACHTER_RUNNER (PRIV (self)->widget));
  if (testcase)
    {
      GFileInfo* info;
      GError   * error = NULL;

      info = g_file_query_info (testcase,
                                G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
                                G_FILE_QUERY_INFO_NONE, NULL,
                                &error);

      if (error)
        {
          g_warning ("error reading display name from file: %s", error->message);
          g_error_free (error);
        }
      else
        {
          title = g_strdup_printf (_("%s - Gutachter"), g_file_info_get_display_name (info));
          g_object_unref (info);
        }
    }

  if (G_LIKELY (PRIV (self)->execute_button))
    {
      /* don't run this after destroy() */
      gtk_widget_set_sensitive (GTK_WIDGET (PRIV (self)->execute_button), testcase != NULL);
    }

  if (title)
    {
      gtk_window_set_title (GTK_WINDOW (self), title);
      g_free (title);
    }
  else
    {
      gtk_window_set_title (GTK_WINDOW (self), _("Gutachter"));
    }
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
