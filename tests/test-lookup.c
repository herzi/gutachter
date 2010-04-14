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

#include <gutachter.h>

static void
test_n_windows (void)
{
  g_assert_cmpint (0, ==, gutachter_lookup_n_windows ());

  gtk_test_create_widget (GTK_TYPE_WINDOW, NULL);

  g_assert_cmpint (1, ==, gutachter_lookup_n_windows ());

  gtk_test_create_widget (GTK_TYPE_WINDOW, NULL);

  g_assert_cmpint (2, ==, gutachter_lookup_n_windows ());
}

static void
test_lookup_windows (void)
{
  GtkWidget* windows[3];
  gchar    * titles[G_N_ELEMENTS (windows)] = {
          "first window",
          "second window",
          "third window"
  };
  size_t i;

  for (i = 0; i < G_N_ELEMENTS (windows); i++)
    {
      windows[i] = gtk_test_create_widget (GTK_TYPE_WINDOW,
                                           "title", titles[i],
                                           NULL);
      gtk_container_add (GTK_CONTAINER (windows[i]), gtk_label_new (titles[i]));
    }

  for (i = 0; i < G_N_ELEMENTS (windows); i++)
    {
      gchar* search_path = g_strdup_printf ("urn:gtk:GtkWindow(\"%s\")", titles[i]);
      g_test_queue_free (search_path);

      gutachter_assert_cmpptr (windows[i], ==, gutachter_lookup_widget (search_path));
    }

  for (i = 0; i < G_N_ELEMENTS (windows); i++)
    {
      gchar* search_path = g_strdup_printf ("urn:gtk:GtkWindow(\"%s\"):GtkWidget[0]", titles[i]);
      g_test_queue_free (search_path);

      gutachter_assert_cmpptr (gtk_bin_get_child (GTK_BIN (windows[i])), ==, gutachter_lookup_widget (search_path));
    }

  /* FIXME: implement filtered lookup with other containers */
}

void
add_tests_for_lookup (void)
{
  g_test_add_func ("/com/github/herzi/gutachter/GutachterLookup/n-windows",
                   test_n_windows);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterLookup/lookup",
                   test_lookup_windows);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
