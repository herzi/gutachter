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

#define PATH_TO_NOTEBOOK       "urn:gtk:GtkWidget[2]"
#define PATH_TO_PANED          PATH_TO_NOTEBOOK ":GtkWidget[0]"
#define PATH_TO_TREEVIEW_FAILS PATH_TO_PANED ":GtkWidget[0]:GtkWidget[0]"
#define PATH_TO_TEXTVIEW       PATH_TO_PANED ":GtkWidget[1]:GtkWidget[0]"

static void
test_layout (void)
{
  GtkWidget* widget = gtk_test_create_widget (GUTACHTER_TYPE_WIDGET, NULL);

  g_assert (GTK_IS_VBOX (widget));
  g_assert (GTK_IS_NOTEBOOK (gutachter_lookup_child (widget, PATH_TO_NOTEBOOK)));
  g_assert (GTK_IS_PANED (gutachter_lookup_child (widget, PATH_TO_PANED)));
  g_assert (GTK_IS_TREE_VIEW (gutachter_lookup_child (widget, PATH_TO_TREEVIEW_FAILS)));
  g_assert (GTK_IS_TEXT_VIEW (gutachter_lookup_child (widget, PATH_TO_TEXTVIEW)));
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
  gutachter_assert_cmpptr (gutachter_lookup_child (widget, PATH_TO_PANED),
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

void
add_tests_for_widget (void)
{
  g_test_add_func (NAMESPACE "GutachterWidget/layout", test_layout);
  g_test_add_func (NAMESPACE "GutachterWidget/display-failure", test_display_failure);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
