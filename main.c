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

#include <gutachter.h>

#define GETTEXT_DOMAIN NULL /* FIXME: enable i18n */
#include <glib/gi18n-lib.h>

int
main (int   argc,
      char**argv)
{
  GtkTestXvfbWrapper* xvfb;
  gchar             ** files = NULL;
  GtkWidget         * window;
  GError            * error = NULL;
  GOptionEntry        entries[] = {
            {G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &files, NULL, NULL},
            {NULL, 0, 0, 0, NULL, NULL, NULL}
  };

  if (!gtk_init_with_args (&argc, &argv, _("[TESTCASE]"),
                           entries, GETTEXT_DOMAIN, &error))
    {
      g_warning ("error initializing application");
      return 1;
    }

  xvfb = gtk_test_xvfb_wrapper_get_instance ();

  window = gtk_test_window_new ();
  if (files && *files)
    {
      GFile* file = g_file_new_for_commandline_arg (*files);
      gtk_test_runner_set_file (GTK_TEST_RUNNER (window), file);
      g_object_unref (file);
    }
  g_strfreev (files);

  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_show (window);

  gtk_main ();

  g_object_unref (xvfb);
  return 0;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
