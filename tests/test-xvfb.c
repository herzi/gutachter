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

static void
test_init (void)
{
  GutachterXvfb* xvfb = gutachter_xvfb_get_instance ();
  g_test_queue_unref (xvfb);

  g_assert (GUTACHTER_IS_XVFB (xvfb));
}

static void
test_wait (void)
{
  GutachterXvfb* xvfb = gutachter_xvfb_get_instance ();
  GdkDisplay   * display;
  GError       * error = NULL;
  gchar        * name;

  g_test_queue_unref (xvfb);

  g_assert (gutachter_xvfb_wait (xvfb, &error));
  g_assert_no_error (error);
  g_assert_cmpint (0, !=, gutachter_xvfb_get_pid (xvfb));

  /* FIXME: it looks like this is easier if we returned a string in the beginning */
  name = g_strdup_printf (":%" G_GUINT64_FORMAT,
                          gutachter_xvfb_get_display (xvfb));

  g_test_queue_free (name);

  display = gdk_display_open (name);
  /* FIXME: try to even create a window on that screen */
  /* FIXME: try to spawn a test on that screen */
  gdk_display_close (display);
}

static gboolean
test_quick_unref_cb (gpointer user_data)
{
  g_main_loop_quit (user_data);

  return FALSE;
}

static void
test_quick_unref (void)
{
  GutachterXvfb* xvfb = gutachter_xvfb_get_instance ();
  GMainLoop    * loop = g_main_loop_new (NULL, FALSE);

  g_test_queue_destroy ((GDestroyNotify)g_main_loop_unref, loop);
  g_object_unref (xvfb);

  g_timeout_add (100, test_quick_unref_cb, loop);
  g_main_loop_run (loop);
}

void
add_tests_for_xvfb (void)
{
  g_test_add_func (NAMESPACE "Xvfb/init", test_init);
  /* singleton */
  g_test_add_func (NAMESPACE "Xvfb/wait", test_wait);
  g_test_add_func (NAMESPACE "Xvfb/quick-unref", test_quick_unref);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
