/* This file is part of herzi's playground
 *
 * Copyright (C) 2010  Sven Herzberg
 *
 * This work is provided "as is"; redistribution and modification
 * in whole or in part, in any medium, physical or electronic is
 * permitted without restriction.
 *
 * This work is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * In no event shall the authors or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 */

#include "test-main.h"

#define PATH_TO_PANED          "urn:gtk:GtkWidget[2]"
#define PATH_TO_NOTEBOOK       PATH_TO_PANED ":GtkWidget[0]"
#define PATH_TO_TEXTVIEW       PATH_TO_PANED ":GtkWidget[1]:GtkWidget[0]"
#define PATH_TO_TREEVIEW_FAILS PATH_TO_NOTEBOOK ":GtkWidget[0]:GtkWidget[0]"
#define PATH_TO_TREEVIEW_TESTS PATH_TO_NOTEBOOK ":GtkWidget[1]:GtkWidget[0]"

static void
test_layout (void)
{
  GtkWidget* widget = gtk_test_create_widget (GUTACHTER_TYPE_WIDGET, NULL);

  g_assert (GTK_IS_VBOX (widget));
  gutachter_assert_child (widget, PATH_TO_PANED, GTK_TYPE_PANED);
  gutachter_assert_child (widget, PATH_TO_NOTEBOOK, GTK_TYPE_NOTEBOOK);
  g_assert (GTK_IS_PANED (gutachter_lookup_child (widget, PATH_TO_PANED)));
  g_assert (GTK_IS_TREE_VIEW (gutachter_lookup_child (widget, PATH_TO_TREEVIEW_FAILS)));
  gutachter_assert_child (widget, PATH_TO_TREEVIEW_TESTS, GTK_TYPE_TREE_VIEW);
  gutachter_assert_child (widget, PATH_TO_TEXTVIEW, GTK_TYPE_TEXT_VIEW);
}

static void
test_set_loaded_iteration (GtkTreeView* view,
                           GtkTreeIter* iter)
{
  GtkTreeModel* model = gtk_tree_view_get_model (view);
  GtkTreeIter   child;

  g_assert (gtk_tree_model_iter_children (model, &child, iter));

  do
    {
      GtkTreePath* path = gtk_tree_model_get_path (model, &child);
      gchar      * path_string = gtk_tree_path_to_string (path);
      if (gtk_tree_model_iter_has_child (model, &child))
        {
          if (!gtk_tree_view_row_expanded (view, path))
            {
              g_warning ("the row at path \"%s\" is not expanded, even though it has at least one child",
                         path_string);
            }
          else
            {
              g_test_message ("%s is expanded", path_string);
              test_set_loaded_iteration (view, &child);
            }
        }
      else
        {
          g_test_message ("%s has no child", path_string);
        }
      g_free (path_string);
      gtk_tree_path_free (path);
    }
  while (gtk_tree_model_iter_next (model, &child));
}

static void
test_set_loaded_cb (GObject   * suite_object,
                    GParamSpec* pspec        G_GNUC_UNUSED,
                    gpointer    user_data)
{
  switch (gutachter_suite_get_status (GUTACHTER_SUITE (suite_object)))
    {
    case GUTACHTER_SUITE_LOADED:
      g_main_loop_quit (user_data);
      break;
    case GUTACHTER_SUITE_ERROR:
      g_assert_not_reached ();
      break;
    case GUTACHTER_SUITE_FINISHED:
      g_main_loop_quit (user_data);
      break;
    default:
      /* ignore */
      break;
    }
}

static void
test_set_loaded (void)
{
  GtkWidget      * cut = gtk_test_create_widget (GUTACHTER_TYPE_WIDGET, NULL);
  GutachterSuite * suite;
  GMainLoop      * loop = g_main_loop_new (NULL, FALSE);
  GFile          * file = g_file_new_for_commandline_arg ("test-pass");
  g_test_queue_destroy ((GDestroyNotify) g_main_loop_unref, loop);
  g_test_queue_unref (file);

  suite = gutachter_suite_new (file);
  g_test_queue_unref (suite);

  g_signal_connect (suite, "notify::status",
                    G_CALLBACK (test_set_loaded_cb), loop);

  gutachter_suite_load (suite);
  g_main_loop_run (loop);

  g_assert_cmpuint (GUTACHTER_SUITE_LOADED, ==, gutachter_suite_get_status (suite));
  gutachter_suite_execute (suite);
  g_assert_cmpuint (GUTACHTER_SUITE_RUNNING, ==, gutachter_suite_get_status (suite));
  gutachter_widget_set_suite (GUTACHTER_WIDGET (cut), suite);

  /* make sure all nodes are expanded */
  test_set_loaded_iteration (GTK_TREE_VIEW (gutachter_lookup_child (cut, PATH_TO_TREEVIEW_TESTS)),
                             NULL);

  //g_main_loop_run (loop); /* wait until the suite has finished running, then return */
}

static void
test_display_failure_cb1 (GObject   * object,
                          GParamSpec* pspec     G_GNUC_UNUSED,
                          gpointer    user_data)
{
  switch (gutachter_suite_get_status (GUTACHTER_SUITE (object)))
    {
    case GUTACHTER_SUITE_LOADING:
    case GUTACHTER_SUITE_RUNNING:
      /* expected and ignored */
      break;
    case GUTACHTER_SUITE_LOADED:
      gutachter_suite_execute (GUTACHTER_SUITE (object));
      break;
    case GUTACHTER_SUITE_FINISHED:
      g_main_loop_quit (user_data);
      break;
    default:
      g_warning ("unexpected state %d",
                 gutachter_suite_get_status (GUTACHTER_SUITE (object)));
      break;
    }
}

static gboolean
test_display_failure_cb2 (gpointer user_data)
{
  g_warn_if_reached ();
  g_main_loop_quit (user_data);
  return FALSE;
}

static void
test_display_failure (void)
{
  GtkWidget* widget = gtk_test_create_widget (GUTACHTER_TYPE_WIDGET, NULL);
  GMainLoop* loop = g_main_loop_new (NULL, FALSE);
  GFile    * file = g_file_new_for_commandline_arg ("test-fail");
  GutachterSuite* suite = gutachter_suite_new (file);
  GtkTextIter     end_iter;
  GtkTextIter     start_iter;
  gchar         * text;

  g_signal_connect (suite, "notify::status",
                    G_CALLBACK (test_display_failure_cb1), loop);

  gutachter_suite_load (suite);

  g_test_queue_unref (file);
  g_test_queue_unref (suite);
  g_test_queue_destroy ((GDestroyNotify)g_main_loop_unref, loop);

  gutachter_widget_set_suite (GUTACHTER_WIDGET (widget), suite);

  g_timeout_add_seconds (5, test_display_failure_cb2, loop);

  g_main_loop_run (loop);

  /* ensure that we switched the active page to the failure list */
  gutachter_assert_cmpptr (gutachter_lookup_child (widget, PATH_TO_NOTEBOOK ":GtkWidget[0]"),
                           ==,
                           gtk_notebook_get_nth_page (GTK_NOTEBOOK (gutachter_lookup_child (widget, PATH_TO_NOTEBOOK)),
                                                      gtk_notebook_get_current_page (GTK_NOTEBOOK (gutachter_lookup_child (widget, PATH_TO_NOTEBOOK)))));

  /* ensure there is a selection in the errors */
  g_assert (gtk_tree_selection_get_selected (gtk_tree_view_get_selection (GTK_TREE_VIEW (gutachter_lookup_child (widget, PATH_TO_TREEVIEW_FAILS))),
                                             NULL,
                                             NULL));

  gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (gutachter_lookup_child (widget, PATH_TO_TEXTVIEW))),
                                  &start_iter);
  gtk_text_buffer_get_end_iter   (gtk_text_view_get_buffer (GTK_TEXT_VIEW (gutachter_lookup_child (widget, PATH_TO_TEXTVIEW))),
                                  &end_iter);
  text = gtk_text_buffer_get_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (gutachter_lookup_child (widget, PATH_TO_TEXTVIEW))),
                                   &start_iter,
                                   &end_iter,
                                   TRUE);
  g_test_queue_free (text);
  g_assert_cmpstr ("ERROR:tests/test-fail.c:28:dummy_fail: assertion failed: (FALSE)",
                   ==, text);
}

static void
test_select_iter (GtkTreeView* treeview,
                  GtkTreeIter* iter)
{
  GtkTreeSelection* selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

  g_assert_cmpuint (GTK_SELECTION_MULTIPLE, !=, gtk_tree_selection_get_mode (selection));

  g_assert_cmpint (0, ==, gtk_tree_selection_count_selected_rows (selection));
  gtk_tree_selection_select_iter (selection, iter);
  g_assert (gtk_tree_selection_iter_is_selected (selection, iter));
  g_assert_cmpint (1, ==, gtk_tree_selection_count_selected_rows (selection));
}

static void
test_display_success_selection (void)
{
  GtkTreeIter       iter;
  GtkTreeStore    * store;
  GtkWidget       * treeview;

  store = gtk_tree_store_new (1, G_TYPE_STRING);
  gtk_tree_store_append (store, &iter, NULL);

  treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
  test_select_iter (GTK_TREE_VIEW (treeview), &iter);

  g_object_unref (store);
}

static gboolean
test_expanded_parents (GtkTreePath* path,
                       GtkTreeView* view)
{
  gchar* string = gtk_tree_path_to_string (path);

  if (gtk_tree_path_get_depth (path) > 1)
    {
      GtkTreePath* parent = gtk_tree_path_copy (path);
      gtk_tree_path_up (parent);
      if (test_expanded_parents (parent, view))
        {
          g_test_message ("%s is visible", string);
        }
      else
        {
          g_warning ("%s is not visible\n", string);
          gtk_tree_path_free (parent);
          g_free (string);
          return FALSE;
        }

      gtk_tree_path_free (parent);
    }

  if (gtk_tree_view_row_expanded (view, path))
    {
      g_test_message ("%s is expanded", string);
      g_free (string);
      return TRUE;
    }

  g_test_message ("%s is not expanded", string);
  g_free (string);
  return FALSE;
}

static void
test_display_success (void)
{
  GtkWidget* widget = gtk_test_create_widget (GUTACHTER_TYPE_WIDGET, NULL);
  GMainLoop* loop = g_main_loop_new (NULL, FALSE);
  GFile    * file = g_file_new_for_commandline_arg ("test-pass");
  GutachterSuite* suite = gutachter_suite_new (file);
  GtkTreeIter     iter;
  GtkTreePath   * path;
  GtkTextIter     end_iter;
  GtkTextIter     start_iter;
  gchar         * text;

  g_signal_connect (suite, "notify::status",
                    G_CALLBACK (test_display_failure_cb1), loop);

  gutachter_suite_load (suite);

  g_test_queue_unref (file);
  g_test_queue_unref (suite);
  g_test_queue_destroy ((GDestroyNotify)g_main_loop_unref, loop);

  g_timeout_add_seconds (5, test_display_failure_cb2, loop);
  g_main_loop_run (loop);

  gutachter_widget_set_suite (GUTACHTER_WIDGET (widget), suite);

  g_assert_cmpint (GUTACHTER_SUITE_FINISHED, ==, gutachter_suite_get_status (suite));

  /* ensure that we switched the active page to the tests list */
  gutachter_assert_cmpptr (gutachter_lookup_child (widget, PATH_TO_NOTEBOOK ":GtkWidget[1]"),
                           ==,
                           gtk_notebook_get_nth_page (GTK_NOTEBOOK (gutachter_lookup_child (widget, PATH_TO_NOTEBOOK)),
                                                      gtk_notebook_get_current_page (GTK_NOTEBOOK (gutachter_lookup_child (widget, PATH_TO_NOTEBOOK)))));

  /* select the first test */
  gutachter_assert_cmpptr (gutachter_suite_get_tree (suite), ==,
                           gtk_tree_view_get_model (GTK_TREE_VIEW (gutachter_lookup_child (widget, PATH_TO_TREEVIEW_TESTS))));
  g_assert (gutachter_hierarchy_lookup_iter (GUTACHTER_HIERARCHY (gutachter_suite_get_tree (suite)),
                                             &iter, "/com/github/herzi/gutachter/test-pass/test-1"));
  path = gtk_tree_model_get_path (gutachter_suite_get_tree (suite), &iter);
  g_assert (path != NULL);
  test_expanded_parents (path, GTK_TREE_VIEW (gutachter_lookup_child (widget, PATH_TO_TREEVIEW_TESTS)));
  gtk_tree_path_free (path);
  g_assert (gtk_tree_store_iter_is_valid (GTK_TREE_STORE (gutachter_suite_get_tree (suite)), &iter));
  test_select_iter (GTK_TREE_VIEW (gutachter_lookup_child (widget, PATH_TO_TREEVIEW_TESTS)), &iter);
  g_assert (gtk_tree_selection_get_selected (gtk_tree_view_get_selection (GTK_TREE_VIEW (gutachter_lookup_child (widget, PATH_TO_TREEVIEW_TESTS))),
                                             NULL,
                                             NULL));

  gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (gutachter_lookup_child (widget, PATH_TO_TEXTVIEW))),
                                  &start_iter);
  gtk_text_buffer_get_end_iter   (gtk_text_view_get_buffer (GTK_TEXT_VIEW (gutachter_lookup_child (widget, PATH_TO_TEXTVIEW))),
                                  &end_iter);
  text = gtk_text_buffer_get_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (gutachter_lookup_child (widget, PATH_TO_TEXTVIEW))),
                                   &start_iter,
                                   &end_iter,
                                   TRUE);
  g_test_queue_free (text);
  g_assert_cmpstr ("successfully passed the test", ==, text);
}

void
add_tests_for_widget (void)
{
  g_test_add_func (NAMESPACE "Widget/layout", test_layout);
  /* FIXME: make sure we expand automatically when an initial suite gets assigned */
  g_test_add_func (NAMESPACE "Widget/set-running", test_set_loaded);
  g_test_add_func (NAMESPACE "Widget/display-failure", test_display_failure);
  g_test_add_func (NAMESPACE "Widget/display-success/GtkTreeSelection", test_display_success_selection);
  g_test_add_func (NAMESPACE "Widget/display-success", test_display_success);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
