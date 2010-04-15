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

#include <stdlib.h> /* exit() */

static void
test_init (void)
{
  GFile         * file = g_file_new_for_path ("test-pass");
  GutachterSuite* suite = gutachter_suite_new (file);

  g_assert (GUTACHTER_IS_SUITE (suite));
  g_assert_cmpint (0, ==, gutachter_suite_get_tests (suite));
  g_assert_cmpint (0, ==, gutachter_suite_get_executed (suite));
  g_assert_cmpint (0, ==, gutachter_suite_get_failures (suite));

  g_object_unref (suite);
  g_object_unref (file);
}

static void
test_load_cb1 (GObject   * object,
               GParamSpec* pspec     G_GNUC_UNUSED,
               gpointer    user_data)
{
  /* quit the main loop when the test is loaded */
  if (gutachter_suite_get_status (GUTACHTER_SUITE (object)) == GUTACHTER_SUITE_LOADED)
    {
      g_main_loop_quit (user_data);
    }
}

static gboolean
test_load_cb2 (gpointer user_data G_GNUC_UNUSED)
{
  /* fail if the test_load_cb1() wasn't called within a second */
  g_assert_not_reached ();

  return FALSE;
}

static void
test_load (void)
{
  GFile         * file = g_file_new_for_path ("test-pass");
  GutachterSuite* suite = gutachter_suite_new (file);
  GMainLoop     * loop = g_main_loop_new (NULL, FALSE);
  gulong          tag;

  g_signal_connect (suite, "notify::status",
                    G_CALLBACK (test_load_cb1), loop);

  gutachter_suite_load (suite);

  tag = g_timeout_add_seconds (1, test_load_cb2, NULL);
  g_main_loop_run (loop);
  g_assert (g_source_remove (tag));

  g_assert_cmpint (1, ==, gutachter_suite_get_tests (suite));
  g_assert_cmpint (0, ==, gutachter_suite_get_executed (suite));
  g_assert_cmpint (0, ==, gutachter_suite_get_failures (suite));

  g_main_loop_unref (loop);
  g_object_unref (suite);
  g_object_unref (file);
}

static void
test_load_missing_cb1 (GObject   * object,
                       GParamSpec* pspec     G_GNUC_UNUSED,
                       gpointer    user_data)
{
  guint* n_runs = user_data;

  g_assert_cmpint (GUTACHTER_SUITE_ERROR, ==, gutachter_suite_get_status (GUTACHTER_SUITE (object)));

  (*n_runs)++;
}

static void
test_load_missing (void)
{
  GFile         * file = g_file_new_for_path ("the-test-that-does-not-exist");
  GutachterSuite* suite = gutachter_suite_new (file);
  guint           n_runs = 0;

  g_assert (!g_file_query_exists (file, NULL));

  g_signal_connect (suite, "notify::status",
                    G_CALLBACK (test_load_missing_cb1), &n_runs);

  gutachter_suite_load (suite);

  g_assert_cmpuint (n_runs, ==, 1);

  g_assert_error (gutachter_suite_get_error (suite),
                  G_SPAWN_ERROR,
                  G_SPAWN_ERROR_NOENT);

  g_assert_cmpint (0, ==, gutachter_suite_get_tests (suite));
  g_assert_cmpint (0, ==, gutachter_suite_get_executed (suite));
  g_assert_cmpint (0, ==, gutachter_suite_get_failures (suite));

  g_object_unref (suite);
  g_object_unref (file);
}

void
add_tests_for_suite (void)
{
  g_test_add_func (NAMESPACE "GutachterSuite/initialize", test_init);
  g_test_add_func (NAMESPACE "GutachterSuite/load", test_load);
  g_test_add_func (NAMESPACE "GutachterSuite/load-missing", test_load_missing);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
