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

#include "gt-widget.h"

#include <gutachter.h>
#include <glib/gi18n.h>

struct _GtkTestWidgetPrivate
{
  GtkWidget   * hierarchy_view;
  GtkWidget   * notebook;
  GtkWidget   * progress;
  gulong        status_handler;
  GtkTestSuite* suite;
};

enum
{
  PROP_0,
  PROP_TEST_SUITE
};

#define PRIV(i) (((GtkTestWidget*)(i))->_private)

static void implement_gtk_test_runner (GtkTestRunnerIface* iface);

G_DEFINE_TYPE_WITH_CODE (GtkTestWidget, gtk_test_widget, GTK_TYPE_VBOX,
                         G_IMPLEMENT_INTERFACE (GTK_TEST_TYPE_RUNNER, implement_gtk_test_runner));

static void
update_sensitivity (GtkTestWidget* self)
{
  GtkProgressBar* progress = GTK_PROGRESS_BAR (PRIV (self)->progress);

  gtk_widget_set_sensitive (PRIV (self)->notebook,
                            PRIV (self)->suite != NULL);

  if (PRIV (self)->suite)
    {
      gtk_progress_bar_pulse (progress);
      gtk_progress_bar_set_text (progress, _("Loading Test Paths..."));
    }
  else
    {
      gtk_progress_bar_set_fraction (progress, 0.0);
      gtk_progress_bar_set_text (progress, _("no test selected"));
    }
}

static void
gtk_test_widget_init (GtkTestWidget* self)
{
  GtkWidget* scrolled;

  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GTK_TEST_TYPE_WIDGET, GtkTestWidgetPrivate);
  PRIV (self)->hierarchy_view = gtk_tree_view_new ();
  PRIV (self)->notebook = gtk_notebook_new ();
  PRIV (self)->progress = gtk_progress_bar_new ();

  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (PRIV (self)->hierarchy_view), -1,
                                              NULL, gtk_cell_renderer_text_new (),
                                              "text", GTK_TEST_HIERARCHY_COLUMN_NAME,
                                              NULL);
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (PRIV (self)->hierarchy_view), -1,
                                              NULL, gtk_cell_renderer_toggle_new (),
                                              "active", GTK_TEST_HIERARCHY_COLUMN_PASSED,
                                              NULL);

  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (PRIV (self)->hierarchy_view), FALSE);

  scrolled = gtk_scrolled_window_new (NULL, NULL);

  gtk_widget_show (PRIV (self)->progress);
  gtk_box_pack_start (GTK_BOX (self), PRIV (self)->progress, FALSE, FALSE, 0);
  /* FIXME: add state information: "Runs: 3/3" "Errors: 2" "Failures: 2" */
  gtk_widget_show (PRIV (self)->hierarchy_view);
  gtk_container_add (GTK_CONTAINER (scrolled), PRIV (self)->hierarchy_view);
  gtk_widget_show (scrolled);
  gtk_container_add_with_properties (GTK_CONTAINER (PRIV (self)->notebook), scrolled,
                                     "tab-label", _("Hierarchy"),
                                     NULL);
  gtk_widget_show (PRIV (self)->notebook);
  /* FIXME: pack the notebook into a paned with the text view */
  gtk_container_add (GTK_CONTAINER (self), PRIV (self)->notebook);
#if 0
  gtk_container_add_with_properties (GTK_CONTAINER (notebook), gtk_label_new ("FAILURES"),
                                     "tab-label", _("Failures"),
                                     NULL);
#endif

  update_sensitivity (self);
}

static void
dispose (GObject* object)
{
  if (PRIV (object)->suite)
    {
      gtk_test_widget_set_suite (GTK_TEST_WIDGET (object), NULL);
    }

  G_OBJECT_CLASS (gtk_test_widget_parent_class)->dispose (object);
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
      g_value_set_object (value, PRIV (object)->suite);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_test_widget_class_init (GtkTestWidgetClass* self_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS (self_class);

  object_class->dispose      = dispose;
  object_class->get_property = get_property;

  g_object_class_override_property (object_class, PROP_TEST_SUITE, "test-suite");

  g_type_class_add_private (self_class, sizeof (GtkTestWidgetPrivate));
}

static GFile*
get_file (GtkTestRunner* runner)
{
  return PRIV (runner)->suite ? gtk_test_suite_get_file (PRIV (runner)->suite) : NULL;
}

static GtkTestSuite*
get_suite (GtkTestRunner* runner)
{
  return PRIV (runner)->suite;
}

static void
set_file (GtkTestRunner* runner,
          GFile        * file)
{
  if (!file && !PRIV (runner)->suite)
    {
      return;
    }

  if (file && PRIV (runner)->suite && file == gtk_test_suite_get_file (PRIV (runner)->suite))
    {
      return;
    }

  if (file)
    {
      GtkTestSuite* suite = gtk_test_suite_new (file);
      gtk_test_widget_set_suite (GTK_TEST_WIDGET (runner), suite);
      g_object_unref (suite);
    }
  else
    {
      gtk_test_widget_set_suite (GTK_TEST_WIDGET (runner), NULL);
    }
}

static void
implement_gtk_test_runner (GtkTestRunnerIface* iface)
{
  iface->get_file  = get_file;
  iface->get_suite = get_suite;
  iface->set_file  = set_file;
}

GtkWidget*
gtk_test_widget_get_hierarchy (GtkTestWidget* self)
{
  g_return_val_if_fail (GTK_TEST_IS_WIDGET (self), NULL);

  return PRIV (self)->hierarchy_view;
}

GtkWidget*
gtk_test_widget_get_notebook (GtkTestWidget* self)
{
  g_return_val_if_fail (GTK_TEST_IS_WIDGET (self), NULL);

  return PRIV (self)->notebook;
}

GtkWidget*
gtk_test_widget_get_progress (GtkTestWidget* self)
{
  g_return_val_if_fail (GTK_TEST_IS_WIDGET (self), NULL);

  return PRIV (self)->progress;
}

GtkWidget*
gtk_test_widget_new (void)
{
  return g_object_new (GTK_TEST_TYPE_WIDGET,
                       NULL);
}

static void
model_changed (GtkTestWidget* self)
{
  switch (gtk_test_suite_get_status (PRIV (self)->suite))
    {
      gchar* text;
    case GUTACHTER_SUITE_LOADING:
    case GUTACHTER_SUITE_LOADED: /* FIXME: finish the process only after regular EOF */
      gtk_progress_bar_pulse (GTK_PROGRESS_BAR (PRIV (self)->progress));
      break;
    case GUTACHTER_SUITE_RUNNING:
    case GUTACHTER_SUITE_FINISHED: /* FIXME: finish the process only after regular EOF */
      text = g_strdup_printf (_("%" G_GUINT64_FORMAT "/%" G_GUINT64_FORMAT),
                              gtk_test_suite_get_executed (PRIV (self)->suite),
                              gtk_test_suite_get_tests (PRIV (self)->suite));
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (PRIV (self)->progress),
                                 text);
      g_free (text);
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (PRIV (self)->progress),
                                     1.0 * gtk_test_suite_get_executed (PRIV (self)->suite) / gtk_test_suite_get_tests (PRIV (self)->suite));
      break;
    default:
      g_return_if_reached ();
      break;
    }
}

static void
row_changed_cb (GtkTreeModel* model     G_GNUC_UNUSED,
                GtkTreePath * path      G_GNUC_UNUSED,
                GtkTreeIter * iter      G_GNUC_UNUSED,
                gpointer      user_data)
{
  model_changed (user_data);
}

static void
row_deleted_cb (GtkTreeModel* model     G_GNUC_UNUSED,
                GtkTreePath * path      G_GNUC_UNUSED,
                gpointer      user_data)
{
  model_changed (user_data);
}

static void
row_inserted_cb (GtkTreeModel* model     G_GNUC_UNUSED,
                 GtkTreePath * path      G_GNUC_UNUSED,
                 GtkTreeIter * iter      G_GNUC_UNUSED,
                 gpointer      user_data)
{
  model_changed (user_data);
}

static void
rows_reordered_cb (GtkTreeModel* model     G_GNUC_UNUSED,
                   GtkTreePath * path      G_GNUC_UNUSED,
                   GtkTreeIter * iter      G_GNUC_UNUSED,
                   guint       * new_order G_GNUC_UNUSED,
                   gpointer      user_data)
{
  model_changed (user_data);
}

static void
status_changed_cb (GObject   * suite     G_GNUC_UNUSED,
                   GParamSpec* pspec     G_GNUC_UNUSED,
                   gpointer    user_data)
{
  GtkTestWidget* self = user_data;

  if (gtk_test_suite_get_status (PRIV (self)->suite) == GUTACHTER_SUITE_LOADED)
    {
      gtk_tree_view_expand_all (GTK_TREE_VIEW (PRIV (self)->hierarchy_view));

      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (PRIV (self)->progress), 0.0);
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (PRIV (self)->progress), _("not tested yet"));
    }
}

void
gtk_test_widget_set_suite (GtkTestWidget* self,
                           GtkTestSuite * suite)
{
  g_return_if_fail (GTK_TEST_IS_WIDGET (self));
  g_return_if_fail (!suite || GTK_TEST_IS_SUITE (suite));

  if (PRIV (self)->suite == suite)
    {
      return;
    }

  if (PRIV (self)->suite)
    {
      g_signal_handler_disconnect (PRIV (self)->suite, PRIV (self)->status_handler);
      gtk_tree_view_set_model (GTK_TREE_VIEW (PRIV (self)->hierarchy_view), NULL);
      g_object_unref (PRIV (self)->suite);
      PRIV (self)->suite = NULL;
    }

  if (suite)
    {
      GtkTreeModel* model = gtk_test_suite_get_tree (suite);

      PRIV (self)->suite = g_object_ref (suite);
      gtk_tree_view_set_model (GTK_TREE_VIEW (PRIV (self)->hierarchy_view),
                               model);

      g_signal_connect (model, "row-changed",
                        G_CALLBACK (row_changed_cb), self);
      g_signal_connect (model, "row-deleted",
                        G_CALLBACK (row_deleted_cb), self);
      g_signal_connect (model, "row-inserted",
                        G_CALLBACK (row_inserted_cb), self);
      g_signal_connect (model, "rows-reordered",
                        G_CALLBACK (rows_reordered_cb), self);

      PRIV (self)->status_handler = g_signal_connect (PRIV (self)->suite, "notify::status",
                                                      G_CALLBACK (status_changed_cb), self);
      status_changed_cb (G_OBJECT (PRIV (self)->suite), NULL, self);
    }

  g_object_notify (G_OBJECT (self), "test-suite");

  update_sensitivity (self);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
