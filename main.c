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
run_test_child_watch (GPid      pid,
                      gint      status,
                      gpointer  user_data)
{
  GtkTestSuite* suite = user_data;
  GIOChannel  * channel = gtk_test_suite_get_channel (suite);

  g_spawn_close_pid (pid);

  if (WIFEXITED (status) && WEXITSTATUS (status))
    {
      g_warning (_("exited with exit code %d"), WEXITSTATUS (status));
    }
  else if (WIFSIGNALED (status))
    {
      g_warning (_("exited with signal %d"), WTERMSIG (status));
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
