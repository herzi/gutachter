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

typedef enum
{
  MODE_LIST,
  MODE_TEST
} RunningMode;

static GtkWidget* window = NULL;
static GtkTestXvfbWrapper* xvfb = NULL;

static GByteArray* buffer = NULL;
static GHashTable* map = NULL;

static guint64 executed = 0;
static guint64 tests = 0;

static GFileMonitor* file_monitor = NULL;

static gboolean
io_func (GIOChannel  * channel,
         GIOCondition  condition G_GNUC_UNUSED,
         gpointer      data      G_GNUC_UNUSED)
{
  guchar  buf[512];
  gsize read_bytes = 0;

  while (G_IO_STATUS_NORMAL == g_io_channel_read_chars (channel, (gchar*)buf, sizeof (buf), &read_bytes, NULL))
    {
      g_byte_array_append (buffer, buf, read_bytes);
    }

  gtk_progress_bar_pulse (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))));
  return TRUE;
}

static gboolean
lookup_iter_for_path (GtkTreeIter* iter,
                      gchar      * path)
{
  GtkTreeRowReference* reference = g_hash_table_lookup (map, path);
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
  g_hash_table_insert (map, path, reference);
  gtk_tree_path_free (tree_path);
}

static void
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
          g_byte_array_append (buffer, (guchar*)data, length);
        }

      tlb = g_test_log_buffer_new ();
      g_test_log_buffer_push (tlb, buffer->len, buffer->data);
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
              tests++;
              break;
            default:
              g_warning ("unexpected message type: %d", msg->log_type);
            }

          g_test_log_msg_free (msg);
        }
      g_test_log_buffer_free (tlb);

      gtk_tree_view_expand_all (GTK_TREE_VIEW (gtk_test_widget_get_hierarchy (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))));

      g_io_channel_unref (channel);
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), 0.0);
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), _("not tested yet"));
    }

  g_spawn_close_pid (pid);
}

static gboolean
run_or_warn (GPid       * pid,
             guint        pipe_id,
             RunningMode  mode)
{
  gboolean  result = FALSE;
  GError  * error  = NULL;
  GFile   * parent = g_file_get_parent (gtk_test_runner_get_file (GTK_TEST_RUNNER (window)));
  gchar   * base;
  gchar   * folder = g_file_get_path (parent);
  gchar   * argv[] = {
          NULL,
          NULL,
          "-q",
          NULL,
          NULL
  };
  gchar** env = g_listenv ();
  gchar** iter;
  gboolean found_display = FALSE;

  base = g_file_get_basename (gtk_test_runner_get_file (GTK_TEST_RUNNER (window)));

  /* FIXME: this is X11 specific */
  for (iter = env; iter && *iter; iter++)
    {
      if (!g_str_has_prefix (*iter, "DISPLAY="))
        {
          g_free (*iter);
          *iter = g_strdup_printf ("DISPLAY=:%" G_GUINT64_FORMAT, gtk_test_xvfb_wrapper_get_display (xvfb));
          found_display = TRUE;
          break;
        }
    }

  if (!found_display)
    {
      gchar** new_env = g_new (gchar*, g_strv_length (env) + 2);
      gchar** new_iter = new_env;

      *new_iter = g_strdup_printf ("DISPLAY=:%" G_GUINT64_FORMAT, gtk_test_xvfb_wrapper_get_display (xvfb));
      for (new_iter++, iter = env; iter && *iter; iter++, new_iter++)
        {
          *new_iter = *iter;
        }
      *new_iter = NULL;

      g_free (env);
      env = new_env;
    }

  switch (mode)
    {
    case MODE_TEST:
      break;
    case MODE_LIST:
      argv[3] = "-l";
      break;
    }

  /* FIXME: should only be necessary on UNIX */
  argv[0] = g_strdup_printf ("./%s", base);
  argv[1] = g_strdup_printf ("--GTestLogFD=%u", pipe_id);

  result = g_spawn_async (folder, argv, env,
                          G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_LEAVE_DESCRIPTORS_OPEN ,//| G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                          NULL, NULL, pid, &error);

  if (!result)
    {
      g_warning ("error executing \"%s\": %s",
                 base, error->message); /* FIXME: use display name */
      g_error_free (error);
    }

  g_free (argv[1]);
  g_free (argv[0]);
  g_free (folder);
  g_object_unref (parent);

  return result;
}

static void
file_changed_cb (GFileMonitor     * monitor    G_GNUC_UNUSED,
                 GFile            * file       G_GNUC_UNUSED,
                 GFile            * other_file G_GNUC_UNUSED,
                 GFileMonitorEvent  event,
                 gpointer           user_data  G_GNUC_UNUSED)
{
  switch (event)
    {
    case G_FILE_MONITOR_EVENT_CHANGED:
    case G_FILE_MONITOR_EVENT_CREATED:
      g_warning ("FIXME: re-scan the file");
      break;
    case G_FILE_MONITOR_EVENT_DELETED:
      g_warning ("FIXME: disable the UI bits for now");
      break;
    default:
      g_print ("file changed: %d\n", event);
      break;
    }
}

static void
selection_changed_cb (GtkWindow* window)
{
  g_hash_table_remove_all (map);
  gtk_tree_store_clear (GTK_TREE_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (gtk_test_widget_get_hierarchy (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))))));

  if (file_monitor)
    {
      g_object_unref (file_monitor);
      file_monitor = NULL;
    }

  if (gtk_test_runner_get_file (GTK_TEST_RUNNER (window)))
    {
      GError* error = NULL;
      gchar* base;
      gchar* title;
      GPid   pid = 0;
      int pipes[2];

      if (pipe (pipes) < 0)
        {
          perror ("pipe()");
          exit (2);
        }

      base = g_file_get_basename (gtk_test_runner_get_file (GTK_TEST_RUNNER (window))); /* FIXME: use the display name */
      title = g_strdup_printf (_("%s - GLib Unit Tests"), base);
      g_free (base);
      gtk_window_set_title (window, title);
      g_free (title);

      tests = 0;
      if (!run_or_warn (&pid, pipes[1], MODE_LIST))
        {
          gtk_widget_set_sensitive (gtk_test_window_get_exec (GTK_TEST_WINDOW (window)), FALSE);
          close (pipes[0]);
        }
      else
        {
          GIOChannel* channel = g_io_channel_unix_new (pipes[0]);
          g_io_channel_set_encoding (channel, NULL, NULL);
          g_io_channel_set_buffered (channel, FALSE);
          g_io_channel_set_flags (channel, G_IO_FLAG_NONBLOCK, NULL);
          buffer = g_byte_array_new ();
          g_io_add_watch (channel, G_IO_IN, io_func, buffer);
          g_child_watch_add (pid, child_watch_cb, channel);
          gtk_widget_set_sensitive (gtk_test_window_get_exec (GTK_TEST_WINDOW (window)), TRUE);

          gtk_progress_bar_pulse (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))));
          gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), _("Loading Test Paths..."));
        }
      close (pipes[1]);

      file_monitor = g_file_monitor (gtk_test_runner_get_file (GTK_TEST_RUNNER (window)), G_FILE_MONITOR_NONE, NULL, &error);
      g_signal_connect (file_monitor, "changed",
                        G_CALLBACK (file_changed_cb), NULL);
    }

  if (!gtk_test_runner_get_file (GTK_TEST_RUNNER (window)))
    {
      gtk_window_set_title (window, _("GLib Unit Tests"));
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), _("no test selected"));
      gtk_widget_set_sensitive (gtk_test_window_get_exec (GTK_TEST_WINDOW (window)), FALSE);
    }
  else
    {
      GFileInfo* info;
      GError   * error = NULL;

      info = g_file_query_info (gtk_test_runner_get_file (GTK_TEST_RUNNER (window)),
                                G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME, G_FILE_QUERY_INFO_NONE, NULL, &error);

      if (error)
        {
          g_warning ("error reading display name from file: %s", error->message);
          g_error_free (error);
        }
      else
        {
          g_object_unref (info);
        }
    }

  gtk_widget_set_sensitive (gtk_test_widget_get_notebook (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window)))),
                            gtk_test_runner_get_file (GTK_TEST_RUNNER (window)) != NULL);
}

static void
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
          g_byte_array_append (buffer, (guchar*)data, length);
        }

      tlb = g_test_log_buffer_new ();
      g_test_log_buffer_push (tlb, buffer->len, buffer->data);
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
              g_warning ("status %d; nforks %d; elapsed %Lf",
                         (int)msg->nums[0], (int)msg->nums[1], msg->nums[2]);
              executed++;
              gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), 1.0 * executed / tests);
              break;
            default:
              g_warning ("unexpected message type: %d", msg->log_type);
            }

          g_test_log_msg_free (msg);
        }
      g_test_log_buffer_free (tlb);

      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), 1.0);
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), _("exited cleanly"));
    }

  g_io_channel_unref (channel);
}

static void
button_clicked_cb (GtkButton* button    G_GNUC_UNUSED,
                   gpointer   user_data G_GNUC_UNUSED)
{
  GPid   pid = 0;
  int    pipes[2];

  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), _("Running tests..."));

  if (pipe (pipes))
    {
      perror ("pipe()");
      return;
    }

  executed = 0;
  if (!run_or_warn (&pid, pipes[1], MODE_TEST))
    {
      close (pipes[0]);
    }
  else
    {
      GIOChannel* channel = g_io_channel_unix_new (pipes[0]);
      g_io_channel_set_encoding (channel, NULL, NULL);
      g_io_channel_set_buffered (channel, FALSE);
      g_io_channel_set_flags (channel, G_IO_FLAG_NONBLOCK, NULL);
      buffer = g_byte_array_new ();
      g_io_add_watch (channel, G_IO_IN, io_func, buffer);
      g_child_watch_add (pid, run_test_child_watch, channel);
      gtk_widget_set_sensitive (gtk_test_window_get_exec (GTK_TEST_WINDOW (window)), TRUE);

      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), 0.0);
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (gtk_test_widget_get_progress (GTK_TEST_WIDGET (gtk_test_window_get_widget (GTK_TEST_WINDOW (window))))), _("Starting Tests..."));
    }

  close (pipes[1]);
}

static void
open_item_clicked (GtkButton* button G_GNUC_UNUSED,
                   GtkWindow* window)
{
  GtkWidget* dialog = gtk_file_chooser_dialog_new (_("Choose Unit Tests"),
                                                   window,
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   GTK_STOCK_CLOSE, GTK_RESPONSE_REJECT,
                                                   GTK_STOCK_OPEN,  GTK_RESPONSE_ACCEPT,
                                                   NULL);
  GFile* file;

  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);

  gtk_dialog_run (GTK_DIALOG (dialog));

  file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
  gtk_test_runner_set_file  (GTK_TEST_RUNNER (window), file);
  g_object_unref (file);

  selection_changed_cb (window);

  gtk_widget_destroy (dialog);
}

int
main (int   argc,
      char**argv)
{
  gtk_init (&argc, &argv);

  xvfb = gtk_test_xvfb_wrapper_new ();

  map = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GFreeFunc)gtk_tree_row_reference_free);

  window = gtk_test_window_new ();

  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  g_signal_connect (gtk_test_window_get_open (GTK_TEST_WINDOW (window)), "clicked",
                    G_CALLBACK (open_item_clicked), window);
  g_signal_connect (gtk_test_window_get_exec (GTK_TEST_WINDOW (window)), "clicked",
                    G_CALLBACK (button_clicked_cb), NULL);

  selection_changed_cb (GTK_WINDOW (window));

  gtk_widget_show (window);

  gtk_main ();

  g_hash_table_destroy (map);
  g_object_unref (xvfb);
  return 0;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
