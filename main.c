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

gboolean
io_func (GIOChannel  * channel,
         GIOCondition  condition G_GNUC_UNUSED,
         gpointer      data      G_GNUC_UNUSED)
{
  guchar  buf[512];
  gsize read_bytes = 0;

  while (G_IO_STATUS_NORMAL == g_io_channel_read_chars (channel, (gchar*)buf, sizeof (buf), &read_bytes, NULL))
    {
      g_byte_array_append (gtk_test_suite_get_buffer (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))), buf, read_bytes);
    }

  gtk_progress_bar_pulse (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))));
  return TRUE;
}

static gboolean
lookup_iter_for_path (GtkTreeIter* iter,
                      gchar      * path)
{
  GtkTreeRowReference* reference = g_hash_table_lookup (gtk_test_suite_get_iter_map (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))), path);
  if (reference)
    {
      GtkTreeStore* store = GTK_TREE_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (gtk_test_widget_get_hierarchy (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window)))))));
      GtkTreePath* tree_path = gtk_tree_row_reference_get_path (reference);
      g_assert (gtk_tree_model_get_iter (GTK_TREE_MODEL (store), iter, tree_path));
      gtk_tree_path_free (tree_path);
      return TRUE;
    }

  return FALSE;
}

static void
create_iter_for_path (GtkTreeIter* iter,
                      gchar      * path)
{
  GtkTreeRowReference* reference;
  GtkTreeStore       * store = GTK_TREE_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (gtk_test_widget_get_hierarchy (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window)))))));
  GtkTreePath        * tree_path;
  gchar              * last_slash;

  if (lookup_iter_for_path (iter, path))
    {
      g_free (path);
      return;
    }

  last_slash = g_strrstr (path, "/");
  if (!last_slash || last_slash == path || *(last_slash + 1) == '\0')
    {
      gtk_tree_store_append (store, iter, NULL);
    }
  else
    {
      GtkTreeIter  parent;

      *last_slash = '\0';
      create_iter_for_path (&parent, g_strdup (path));
      *last_slash = '/';

      last_slash++;

      gtk_tree_store_append (store, iter, &parent);
    }

  gtk_tree_store_set (store, iter,
                      GTK_TEST_HIERARCHY_COLUMN_PASSED, FALSE,
                      GTK_TEST_HIERARCHY_COLUMN_NAME, last_slash,
                      -1);

  tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (store), iter);
  reference = gtk_tree_row_reference_new (GTK_TREE_MODEL (store), tree_path);
  g_hash_table_insert (gtk_test_suite_get_iter_map (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))), path, reference);
  gtk_tree_path_free (tree_path);
}

void
child_watch_cb (GPid      pid,
                gint      status,
                gpointer  data)
{
  if (WIFEXITED (status) && WEXITSTATUS (status) != 0)
    {
      g_warning ("child exited with error code: %d", WEXITSTATUS (status));
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
    }
  else
    {
      GTestLogMsg *msg;
      GTestLogBuffer* tlb;
      GIOChannel* channel = data;
      GError* error = NULL;
      gsize length = 0;
      gchar* data = NULL;
      GIOStatus  status;

      while (G_IO_STATUS_NORMAL == (status = g_io_channel_read_to_end (channel, &data, &length, &error)))
        {
          g_byte_array_append (gtk_test_suite_get_buffer (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))), (guchar*)data, length);
        }

      tlb = g_test_log_buffer_new ();
      g_test_log_buffer_push (tlb,
                              gtk_test_suite_get_buffer (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window)))->len,
                              gtk_test_suite_get_buffer (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window)))->data);
      for (msg = g_test_log_buffer_pop (tlb); msg; msg = g_test_log_buffer_pop (tlb))
        {
          GtkTreeIter  iter;
          gchar      * path;

          switch (msg->log_type)
            {
            case G_TEST_LOG_START_BINARY:
              break;
            case G_TEST_LOG_LIST_CASE:
              path = g_strdup (msg->strings[0]);;
              create_iter_for_path (&iter, path);
              gtk_test_suite_set_tests (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window)),
                                        1 + gtk_test_suite_get_tests (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))));
              break;
            default:
              g_warning ("unexpected message type: %d", msg->log_type);
            }

          g_test_log_msg_free (msg);
        }
      g_test_log_buffer_free (tlb);
      g_byte_array_set_size (gtk_test_suite_get_buffer (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))), 0);

      gtk_tree_view_expand_all (GTK_TREE_VIEW (gtk_test_widget_get_hierarchy (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))));

      g_io_channel_unref (channel);
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), 0.0);
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), _("not tested yet"));
    }

  g_spawn_close_pid (pid);
}

void
run_test_child_watch (GPid      pid,
                      gint      status,
                      gpointer  user_data)
{
  GIOChannel* channel = user_data;

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
      GtkTreeStore* store = GTK_TREE_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (gtk_test_widget_get_hierarchy (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window)))))));
      GTestLogMsg *msg;
      GTestLogBuffer* tlb;
      GError* error = NULL;
      gsize length = 0;
      gchar* data = NULL;
      GIOStatus  status;
      GtkTreeIter  iter;

      while (G_IO_STATUS_NORMAL == (status = g_io_channel_read_to_end (channel, &data, &length, &error)))
        {
          g_byte_array_append (gtk_test_suite_get_buffer (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))), (guchar*)data, length);
        }

      tlb = g_test_log_buffer_new ();
      g_test_log_buffer_push (tlb,
                              gtk_test_suite_get_buffer (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window)))->len,
                              gtk_test_suite_get_buffer (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window)))->data);
      for (msg = g_test_log_buffer_pop (tlb); msg; msg = g_test_log_buffer_pop (tlb))
        {
          switch (msg->log_type)
            {
            case G_TEST_LOG_START_BINARY:
              break;
            case G_TEST_LOG_START_CASE:
              lookup_iter_for_path (&iter, msg->strings[0]);
              break;
            case G_TEST_LOG_STOP_CASE:
              gtk_tree_store_set (store, &iter,
                                  GTK_TEST_HIERARCHY_COLUMN_PASSED, msg->nums[0] == 0,
                                  -1);
              g_message ("status %d; nforks %d; elapsed %Lf",
                         (int)msg->nums[0], (int)msg->nums[1], msg->nums[2]);
              gtk_test_suite_set_executed (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window)),
                                           1 + gtk_test_suite_get_executed (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))));
              gchar* text = g_strdup_printf (_("%" G_GUINT64_FORMAT "/%" G_GUINT64_FORMAT),
                                      gtk_test_suite_get_executed (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))),
                                      gtk_test_suite_get_tests (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))));
              gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))),
                                        text);
              g_print ("%s\n", text);
              g_free (text);
              gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))),
                                             1.0 * gtk_test_suite_get_executed (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))) / gtk_test_suite_get_tests (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))));
              break;
            default:
              g_warning ("%s(%s): unexpected message type: %d",
                         G_STRFUNC, G_STRLOC,
                         msg->log_type);
            }

          g_test_log_msg_free (msg);
        }
      g_test_log_buffer_free (tlb);
      g_byte_array_set_size (gtk_test_suite_get_buffer (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))), 0);

      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), 1.0);
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), _("exited cleanly"));
    }

  g_io_channel_unref (channel);
  gtk_widget_set_sensitive (gtk_test_window_get_exec (GTK_TEST_WINDOW (window)), TRUE);
}

static void
button_clicked_cb (GtkButton* button    G_GNUC_UNUSED,
                   gpointer   user_data)
{
  GtkTestWindow* window = user_data;
  GPid           pid = 0;
  int            pipes[2];

  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (window)))),
                             _("Running tests..."));

  if (pipe (pipes))
    {
      perror ("pipe()");
      return;
    }

  gtk_test_suite_set_executed (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window)), 0);
  if (!run_or_warn (&pid, pipes[1], MODE_TEST, gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))))
    {
      close (pipes[0]);
    }
  else
    {
      GIOChannel* channel = g_io_channel_unix_new (pipes[0]);
      g_io_channel_set_encoding (channel, NULL, NULL);
      g_io_channel_set_buffered (channel, FALSE);
      g_io_channel_set_flags (channel, G_IO_FLAG_NONBLOCK, NULL);
      g_io_add_watch (channel, G_IO_IN, io_func, gtk_test_suite_get_buffer (gtk_test_runner_get_suite (GTK_TEST_RUNNER (window))));
      g_child_watch_add (pid, run_test_child_watch, channel);
      gtk_widget_set_sensitive (gtk_test_window_get_exec (GTK_TEST_WINDOW (window)), FALSE);

      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), 0.0);
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))),
                                 _("Starting Tests..."));
    }

  close (pipes[1]);
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

  g_signal_connect (gtk_test_window_get_exec (GTK_TEST_WINDOW (window)), "clicked",
                    G_CALLBACK (button_clicked_cb), window);

  gtk_widget_show (window);

  gtk_main ();

  g_object_unref (xvfb);
  return 0;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
