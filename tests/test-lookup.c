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
      if (i)
        {
          gtk_container_add (GTK_CONTAINER (windows[i]), gtk_label_new (titles[i]));
        }
      else
        {
          GtkWidget* box = gtk_hbox_new (FALSE, 6);

          gtk_container_add (GTK_CONTAINER (box), gtk_label_new (titles[i]));
          gtk_container_add (GTK_CONTAINER (box), gtk_entry_new ());
          gtk_container_add (GTK_CONTAINER (box), gtk_button_new ());
          gtk_container_add (GTK_CONTAINER (windows[i]), box);
        }
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

  /* FIXME: implement and test this: use "…:[0]:…" as a shortcut for any-child -- which is what we currently implement */
  /* FIXME: change the following line to "urn:gtk:GtkWindow(\"third window\"):[0]:GtkLabel[0]" and compare with the current one */
  g_assert (GTK_IS_LABEL (gutachter_lookup_widget ("urn:gtk:GtkWindow(\"first window\"):GtkWidget[0]:GtkWidget[0]")));
  /* FIXME: change the following line to "urn:gtk:GtkWindow(\"third window\"):[0]:GtkEntry[0]" and compare with the current one */
  g_assert (GTK_IS_ENTRY (gutachter_lookup_widget ("urn:gtk:GtkWindow(\"first window\"):GtkWidget[0]:GtkWidget[1]")));
  /* FIXME: change the following line to "urn:gtk:GtkWindow(\"third window\"):[0]:GtkButton[0]" and compare with the current one */
  g_assert (GTK_IS_BUTTON (gutachter_lookup_widget ("urn:gtk:GtkWindow(\"first window\"):GtkWidget[0]:GtkWidget[2]")));
}

static void
test_lookup_child (void)
{
  GtkWidget* hbox = gtk_test_create_widget (GTK_TYPE_HBOX, NULL);
  gtk_container_add (GTK_CONTAINER (hbox), gtk_label_new (NULL));
  gtk_container_add (GTK_CONTAINER (hbox), gtk_entry_new ());
  gtk_container_add (GTK_CONTAINER (hbox), gtk_button_new ());

  gutachter_assert_cmpptr (NULL, ==, gutachter_lookup_child (NULL, ""));
  gutachter_assert_cmpptr (hbox, ==, gutachter_lookup_child (hbox, ""));
  g_assert (GTK_IS_LABEL (gutachter_lookup_child (hbox, "urn:gtk:GtkWidget[0]")));
  g_assert (GTK_IS_ENTRY (gutachter_lookup_child (hbox, "urn:gtk:GtkWidget[1]")));
  g_assert (GTK_IS_BUTTON (gutachter_lookup_child (hbox, "urn:gtk:GtkWidget[2]")));
}

void
add_tests_for_lookup (void)
{
  g_test_add_func ("/com/github/herzi/gutachter/GutachterLookup/n-windows",
                   test_n_windows);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterLookup/lookup",
                   test_lookup_windows);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterLookup/lookup-child",
                   test_lookup_child);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
