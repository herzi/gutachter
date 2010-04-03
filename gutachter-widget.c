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

#include "gutachter-widget.h"

#include <gutachter.h>
#include <glib/gi18n.h>

struct _GtkTestWidgetPrivate
{
  GtkWidget     * error_text_view;
  GtkWidget     * failure_view;
  GtkWidget     * hierarchy_view;
  GtkWidget     * indicator_bar;
  GtkWidget     * label_failures;
  GtkWidget     * notebook;
  GtkWidget     * progress;
  gulong          status_handler;
  GutachterSuite* suite;
};

enum
{
  PROP_0,
  PROP_TEST_SUITE
};

#define PRIV(i) (((GtkTestWidget*)(i))->_private)

static void implement_gutachter_runner (GutachterRunnerIface* iface);

G_DEFINE_TYPE_WITH_CODE (GtkTestWidget, gtk_test_widget, GTK_TYPE_VBOX,
                         G_IMPLEMENT_INTERFACE (GUTACHTER_TYPE_RUNNER, implement_gutachter_runner));

static void
update_sensitivity (GtkTestWidget* self)
{
  GtkProgressBar* progress = GTK_PROGRESS_BAR (PRIV (self)->progress);

  gtk_widget_set_sensitive (PRIV (self)->notebook,
                            PRIV (self)->suite != NULL);

  if (PRIV (self)->suite)
    {
      if (gutachter_suite_get_status (PRIV (self)->suite) == GUTACHTER_SUITE_INDETERMINED)
        {
          gtk_progress_bar_set_fraction (progress, 0.0);
        }
      else
        {
          gtk_progress_bar_pulse (progress);
          gtk_progress_bar_set_text (progress, _("Loading Test Paths..."));
        }
    }
  else
    {
      gtk_progress_bar_set_fraction (progress, 0.0);
      gtk_progress_bar_set_text (progress, _("no test selected"));
    }
}

static void
pixbuf_data_func (GtkTreeViewColumn* column    G_GNUC_UNUSED,
                  GtkCellRenderer  * renderer,
                  GtkTreeModel     * model,
                  GtkTreeIter      * iter,
                  gpointer           user_data G_GNUC_UNUSED)
{
  gboolean  unsure = TRUE;
  gboolean  passed = TRUE;

  gtk_tree_model_get (model, iter,
                      GUTACHTER_HIERARCHY_COLUMN_PASSED, &passed,
                      GUTACHTER_HIERARCHY_COLUMN_UNSURE, &unsure,
                      -1);

  g_object_set (renderer,
                "stock-id", unsure ? GTK_STOCK_DIALOG_QUESTION : passed ? GTK_STOCK_APPLY : GTK_STOCK_CANCEL,
                NULL);
}

static void
full_path_data_func (GtkTreeViewColumn* column    G_GNUC_UNUSED,
                     GtkCellRenderer  * renderer,
                     GtkTreeModel     * model,
                     GtkTreeIter      * iter,
                     gpointer           user_data G_GNUC_UNUSED)
{
  GutachterTreeList* tree_list;
  GtkTreeIter        hierarchy_iter;
  GtkTreeIter        tree_list_iter;
  gchar            * full_path = NULL;

  gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER (model), &tree_list_iter, iter);
  tree_list = GUTACHTER_TREE_LIST (gtk_tree_model_filter_get_model (GTK_TREE_MODEL_FILTER (model)));

  g_return_if_fail (gutachter_tree_list_iter_to_child (tree_list, &hierarchy_iter, &tree_list_iter));

  full_path = gutachter_hierarchy_get_full_path (GUTACHTER_HIERARCHY (gutachter_tree_list_get_model (tree_list)), &hierarchy_iter);

  g_object_set (renderer,
                "text", full_path,
                NULL);

  g_free (full_path);
}

static void
failure_selection_changed (GtkTreeSelection* selection,
                           gpointer          user_data)
{
  GtkTestWidget* self = user_data;
  GtkTreeModel * model;
  GtkTreeIter    iter;

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      GutachterTreeList * tree_list = GUTACHTER_TREE_LIST (gtk_tree_model_filter_get_model (GTK_TREE_MODEL_FILTER (model)));
      GutachterHierarchy* hierarchy = GUTACHTER_HIERARCHY (gutachter_tree_list_get_model (tree_list));
      GtkTreeIter         tree_list_iter;
      GtkTreeIter         hierarchy_iter;
      gchar* error_message;

      gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER (model), &tree_list_iter, &iter);
      g_return_if_fail (gutachter_tree_list_iter_to_child (tree_list, &hierarchy_iter, &tree_list_iter));
      error_message = gutachter_hierarchy_get_message (hierarchy, &hierarchy_iter);
      gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (PRIV (self)->error_text_view)),
                                error_message, -1);
      g_free (error_message);
    }
  else
    {
      gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (PRIV (self)->error_text_view)),
                                "", -1);
    }
}

static void
gtk_test_widget_init (GtkTestWidget* self)
{
  GtkTreeViewColumn* column;
  GtkCellRenderer  * renderer;
  GtkWidget        * scrolled;
  GtkWidget        * paned;

  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GTK_TEST_TYPE_WIDGET, GtkTestWidgetPrivate);
  PRIV (self)->error_text_view = gtk_text_view_new ();
  PRIV (self)->failure_view = gtk_tree_view_new ();
  PRIV (self)->hierarchy_view = gtk_tree_view_new ();
  PRIV (self)->indicator_bar = gutachter_bar_new ();
  PRIV (self)->label_failures = gtk_label_new (NULL);
  PRIV (self)->notebook = gtk_notebook_new ();
  PRIV (self)->progress = gtk_progress_bar_new ();

  g_signal_connect (gtk_tree_view_get_selection (GTK_TREE_VIEW (PRIV (self)->failure_view)), "changed",
                    G_CALLBACK (failure_selection_changed), self);

  column = gtk_tree_view_column_new ();
  gtk_tree_view_column_set_expand (column, TRUE);
  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer,
                "ellipsize", PANGO_ELLIPSIZE_MIDDLE,
                NULL);
  gtk_tree_view_column_pack_start (column, renderer, TRUE);
  gtk_tree_view_column_set_cell_data_func (column, renderer,
                                           full_path_data_func, NULL,
                                           NULL);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (PRIV (self)->failure_view), column, -1);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (PRIV (self)->failure_view), FALSE);

  column = gtk_tree_view_column_new ();
  gtk_tree_view_column_set_expand (column, TRUE);
  renderer = gtk_cell_renderer_pixbuf_new ();
  gtk_tree_view_column_pack_start (column, renderer, FALSE);
  gtk_tree_view_column_set_cell_data_func (column, renderer,
                                           pixbuf_data_func, NULL,
                                           NULL);
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (column, renderer, TRUE);
  gtk_tree_view_column_set_attributes (column, renderer,
                                       "text", GUTACHTER_HIERARCHY_COLUMN_NAME,
                                       NULL);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (PRIV (self)->hierarchy_view), column, -1);

  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (PRIV (self)->hierarchy_view), FALSE);

  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (PRIV (self)->error_text_view), GTK_WRAP_WORD_CHAR);

  gtk_widget_show (PRIV (self)->progress);
  gtk_box_pack_start (GTK_BOX (self), PRIV (self)->progress, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (PRIV (self)->indicator_bar), PRIV (self)->label_failures);
  gtk_widget_show_all (PRIV (self)->indicator_bar);
  gtk_box_pack_start (GTK_BOX (self), PRIV (self)->indicator_bar, FALSE, FALSE, 0);

  gtk_widget_show (PRIV (self)->failure_view);
  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (scrolled), PRIV (self)->failure_view);
  gtk_widget_show (scrolled);
  paned = gtk_vpaned_new ();
  gtk_paned_pack1 (GTK_PANED (paned), scrolled,
                   TRUE, FALSE);
  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (PRIV (self)->error_text_view);
  gtk_container_add (GTK_CONTAINER (scrolled), PRIV (self)->error_text_view);
  gtk_widget_show (scrolled);
  gtk_paned_pack2 (GTK_PANED (paned),
                   scrolled,
                   FALSE, FALSE);
  gtk_widget_show (paned);
  gtk_container_add_with_properties (GTK_CONTAINER (PRIV (self)->notebook), paned,
                                     "tab-label", _("Failures"),
                                     NULL);

  gtk_widget_show (PRIV (self)->hierarchy_view);
  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (scrolled), PRIV (self)->hierarchy_view);
  gtk_widget_show (scrolled);
  gtk_container_add_with_properties (GTK_CONTAINER (PRIV (self)->notebook), scrolled,
                                     "tab-label", _("Hierarchy"),
                                     NULL);

  gtk_notebook_set_current_page (GTK_NOTEBOOK (PRIV (self)->notebook), 1);
  gtk_widget_show_all (PRIV (self)->notebook);

  gtk_container_add (GTK_CONTAINER (self), PRIV (self)->notebook);

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
get_file (GutachterRunner* runner)
{
  return PRIV (runner)->suite ? gutachter_suite_get_file (PRIV (runner)->suite) : NULL;
}

static GutachterSuite*
get_suite (GutachterRunner* runner)
{
  return PRIV (runner)->suite;
}

static void
set_file (GutachterRunner* runner,
          GFile          * file)
{
  if (!file && !PRIV (runner)->suite)
    {
      return;
    }

  if (file && PRIV (runner)->suite && file == gutachter_suite_get_file (PRIV (runner)->suite))
    {
      return;
    }

  if (file)
    {
      GutachterSuite* suite = gutachter_suite_new (file);
      gtk_test_widget_set_suite (GTK_TEST_WIDGET (runner), suite);
      g_object_unref (suite);
    }
  else
    {
      gtk_test_widget_set_suite (GTK_TEST_WIDGET (runner), NULL);
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
  switch (gutachter_suite_get_status (PRIV (self)->suite))
    {
      gchar* text;
    case GUTACHTER_SUITE_LOADING:
    case GUTACHTER_SUITE_LOADED: /* FIXME: finish the process only after regular EOF */
      gtk_progress_bar_pulse (GTK_PROGRESS_BAR (PRIV (self)->progress));
      break;
    case GUTACHTER_SUITE_RUNNING:
    case GUTACHTER_SUITE_FINISHED: /* FIXME: finish the process only after regular EOF */
      text = g_strdup_printf (_("%" G_GUINT64_FORMAT "/%" G_GUINT64_FORMAT),
                              gutachter_suite_get_executed (PRIV (self)->suite),
                              gutachter_suite_get_tests (PRIV (self)->suite));
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (PRIV (self)->progress),
                                 text);
      g_free (text);
      if (!gutachter_suite_get_passed (PRIV (self)->suite) ||
          gutachter_suite_get_executed (PRIV (self)->suite) >= gutachter_suite_get_tests (PRIV (self)->suite))
        {
          gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (PRIV (self)->progress), 1.0);
        }
      else
        {
          gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (PRIV (self)->progress),
                                         1.0 * gutachter_suite_get_executed (PRIV (self)->suite) / gutachter_suite_get_tests (PRIV (self)->suite));
        }

      text = g_strdup_printf (g_dngettext (GETTEXT_PACKAGE,
                                           "%" G_GUINT64_FORMAT " failure",
                                           "%" G_GUINT64_FORMAT " failures",
                                           gutachter_suite_get_failures (PRIV (self)->suite)),
                              gutachter_suite_get_failures (PRIV (self)->suite));
      gtk_label_set_text (GTK_LABEL (PRIV (self)->label_failures), text);
      g_free (text);

      gutachter_bar_set_okay (GUTACHTER_BAR (PRIV (self)->indicator_bar),
                              gutachter_suite_get_passed (PRIV (self)->suite));
      break;
    case GUTACHTER_SUITE_INDETERMINED:
      update_sensitivity (self);
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

  switch (gutachter_suite_get_status (PRIV (self)->suite))
    {
    case GUTACHTER_SUITE_LOADED:
      gtk_tree_view_expand_all (GTK_TREE_VIEW (PRIV (self)->hierarchy_view));

      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (PRIV (self)->progress), 0.0);
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (PRIV (self)->progress), _("not tested yet"));
      break;
    case GUTACHTER_SUITE_FINISHED:
      /* let the bar be green in the beginning */
      gutachter_bar_set_okay (GUTACHTER_BAR (PRIV (self)->indicator_bar),
                              gutachter_suite_get_passed (PRIV (self)->suite));

      if (!gutachter_suite_get_passed (PRIV (self)->suite))
        {
          gtk_notebook_set_current_page (GTK_NOTEBOOK (PRIV (self)->notebook), 0);
        }
      else
        {
          gtk_notebook_set_current_page (GTK_NOTEBOOK (PRIV (self)->notebook), 1);
        }
      break;
    default:
      break;
    }
}

static gboolean
failure_visible_func (GtkTreeModel* model,
                      GtkTreeIter * iter,
                      gpointer      user_data G_GNUC_UNUSED)
{
  GutachterHierarchy* hierarchy = GUTACHTER_HIERARCHY (gutachter_tree_list_get_model (GUTACHTER_TREE_LIST (model)));
  GtkTreeIter         child_iter;
  gboolean            passed = TRUE;
  gboolean            unsure = TRUE;
  gboolean            is_testcase = FALSE;

  gtk_tree_model_get (model, iter,
                      GUTACHTER_HIERARCHY_COLUMN_UNSURE, &unsure,
                      GUTACHTER_HIERARCHY_COLUMN_PASSED, &passed,
                      -1);

  if (gutachter_tree_list_iter_to_child (GUTACHTER_TREE_LIST (model), &child_iter, iter))
    {
      is_testcase = gutachter_hierarchy_is_testcase (hierarchy, &child_iter);
    }
  else
    {
      g_warning ("Eeeek!");
    }

  return is_testcase && !unsure && !passed;
}

void
gtk_test_widget_set_suite (GtkTestWidget * self,
                           GutachterSuite* suite)
{
  g_return_if_fail (GTK_TEST_IS_WIDGET (self));
  g_return_if_fail (!suite || GUTACHTER_IS_SUITE (suite));

  if (PRIV (self)->suite == suite)
    {
      return;
    }

  if (PRIV (self)->suite)
    {
      g_signal_handler_disconnect (PRIV (self)->suite, PRIV (self)->status_handler);
      gtk_tree_view_set_model (GTK_TREE_VIEW (PRIV (self)->failure_view), NULL);
      gtk_tree_view_set_model (GTK_TREE_VIEW (PRIV (self)->hierarchy_view), NULL);
      g_object_unref (PRIV (self)->suite);
      PRIV (self)->suite = NULL;
    }

  if (suite)
    {
      GtkTreeModel* model = gutachter_suite_get_tree (suite);

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

      model = gutachter_tree_list_new (model);
      gtk_tree_view_set_model (GTK_TREE_VIEW (PRIV (self)->failure_view),
                               gtk_tree_model_filter_new (model, NULL));
      g_object_unref (model);
      model = gtk_tree_view_get_model (GTK_TREE_VIEW (PRIV (self)->failure_view));
      gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER (model),
                                              failure_visible_func,
                                              NULL, NULL);
      g_object_unref (model);

      PRIV (self)->status_handler = g_signal_connect (PRIV (self)->suite, "notify::status",
                                                      G_CALLBACK (status_changed_cb), self);
      status_changed_cb (G_OBJECT (PRIV (self)->suite), NULL, self);
    }

  g_object_notify (G_OBJECT (self), "test-suite");

  update_sensitivity (self);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
