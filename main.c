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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h> /* WIFEXITED() */
#include <unistd.h>

#include <gtk-test.h>

#include <glib/gi18n.h>

static GtkWidget* window = NULL;

void
child_watch_cb (GPid      pid,
                gint      status,
                gpointer  data)
{
  GtkTestSuite* suite = data;

  g_spawn_close_pid (pid);

  if (WIFEXITED (status) && WEXITSTATUS (status) != 0)
    {
      g_warning ("child exited with error code: %d", WEXITSTATUS (status));
      gtk_test_suite_set_status (suite, GUTACHTER_SUITE_INDETERMINED);
    }
  else if (!WIFEXITED (status))
    {
      if (WIFSIGNALED (status))
        {
          g_warning ("child exited with signal %d", WTERMSIG (status));
        }
      else
        {
          g_warning ("child didn't exit normally: %d", status);
        }
      gtk_test_suite_set_status (suite, GUTACHTER_SUITE_INDETERMINED);
    }
  else
    {
      GTestLogBuffer* tlb = gtk_test_suite_get_buffer (suite);
      GIOChannel* channel = gtk_test_suite_get_channel (suite);
      GError* error = NULL;
      gsize length = 0;
      gchar* data = NULL;
      GIOStatus  status;

      /* FIXME: try calling into io_func() */
      while (G_IO_STATUS_NORMAL == (status = g_io_channel_read_to_end (channel, &data, &length, &error)))
        {
          g_test_log_buffer_push (tlb, length, (guchar*)data);
        }

      gtk_test_suite_read_available (suite);
      /* FIXME: warn if there's unparsed data */
      g_string_set_size (tlb->data, 0);

      gtk_test_suite_set_channel (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window)), NULL);
      gtk_test_suite_set_status (suite, GUTACHTER_SUITE_LOADED);
    }
}

void
run_test_child_watch (GPid      pid,
                      gint      status,
                      gpointer  user_data)
{
  GtkTestSuite* suite = user_data;
  GIOChannel  * channel = gtk_test_suite_get_channel (suite);

  g_spawn_close_pid (pid);

  if (WIFEXITED (status) && WEXITSTATUS (status))
    {
      gchar* text = g_strdup_printf (_("exited with exit code %d"), WEXITSTATUS (status));
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), text);
      g_free (text);
    }
  else if (WIFSIGNALED (status))
    {
      gchar* text = g_strdup_printf (_("exited with signal %d"), WTERMSIG (status));
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), text);
      g_free (text);
    }
  else if (WIFEXITED (status))
    {
      GTestLogBuffer* tlb = gtk_test_suite_get_buffer (suite);
      GError* error = NULL;
      gsize length = 0;
      gchar* data = NULL;
      GIOStatus  status;

      /* FIXME: try calling into io_func() */
      while (G_IO_STATUS_NORMAL == (status = g_io_channel_read_to_end (channel, &data, &length, &error)))
        {
          g_test_log_buffer_push (tlb, length, (guchar*)data);
        }

      gtk_test_suite_read_available (suite);
      /* FIXME: warn if there's unparsed data */
      g_string_set_size (tlb->data, 0);
    }

  gtk_test_suite_set_status (suite, GUTACHTER_SUITE_FINISHED);
  gtk_widget_set_sensitive (gtk_test_window_get_exec (GTK_TEST_WINDOW (window)), TRUE);
}

int
main (int   argc,
      char**argv)
{
  GtkTestXvfbWrapper* xvfb;

  gtk_init (&argc, &argv);

  xvfb = gtk_test_xvfb_wrapper_get_instance ();

  window = gtk_test_window_new ();

  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_show (window);

  gtk_main ();

  g_object_unref (xvfb);
  return 0;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
