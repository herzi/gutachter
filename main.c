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
#include <sys/wait.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include <glib/gi18n.h>

enum
{
  COL_NAME,
  N_COLUMNS
};

static GtkWidget* button_run = NULL;
static GtkWidget* progress = NULL;
static GtkWidget* notebook = NULL;

static GByteArray* buffer = NULL;
static GHashTable* map = NULL;
static GtkTreeStore* store = NULL;

static gboolean
io_func (GIOChannel* channel,
         GIOCondition condition,
         gpointer data)
{
  guchar  buf[512];
  gsize read_bytes = 0;
  while (G_IO_STATUS_NORMAL == g_io_channel_read_chars (channel, (gchar*)buf, sizeof (buf), &read_bytes, NULL))
    {
      g_byte_array_append (buffer, buf, read_bytes);
    }
  return TRUE;
}

static void
create_iter_for_path (GtkTreeIter* iter,
                      gchar      * path)
{
  GtkTreePath* tree_path;
  gchar      * last_slash;

  if (g_hash_table_lookup (map, path))
    {
      tree_path = gtk_tree_row_reference_get_path (g_hash_table_lookup (map, path));
      g_assert (gtk_tree_model_get_iter (GTK_TREE_MODEL (store), iter, tree_path));
      gtk_tree_path_free (tree_path);
      g_free (path);
      return;
    }

  last_slash = g_strrstr (path, "/");
  if (!last_slash || last_slash == path || *(last_slash + 1) == '\0')
    {
      gtk_tree_store_append (store, iter, NULL);
      gtk_tree_store_set (store, iter,
                          COL_NAME, path,
                          -1);
    }
  else
    {
      GtkTreeIter  parent;

      *last_slash = '\0';
      create_iter_for_path (&parent, path);
      *last_slash = '/';

      last_slash++;

      gtk_tree_store_append (store, iter, &parent);
      gtk_tree_store_set (store, iter,
                          COL_NAME, last_slash,
                          -1);
    }

  tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (store), iter);
  g_hash_table_insert (map, path, gtk_tree_row_reference_new (GTK_TREE_MODEL (store), tree_path));
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
      g_warning ("child didn't exit normally: %d", status);
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
              break;
            default:
              g_warning ("unexpected message type: %d", msg->log_type);
            }

          g_test_log_msg_free (msg);
        }
      g_test_log_buffer_free (tlb);

      g_io_channel_unref (channel);
    }

  g_spawn_close_pid (pid);
}

static void
selection_changed_cb (GtkFileChooser* chooser,
                      GtkWindow     * window)
{
  GFile* selected = gtk_file_chooser_get_file (chooser);

  if (selected)
    {
      GFile* parent = g_file_get_parent (selected);
      gchar* base = g_file_get_basename (selected); /* FIXME: use the display name */
      gchar* title = g_strdup_printf (_("%s - GLib Unit Tests"), base);
      int pipes[2];

      if (pipe (pipes) < 0)
        {
          perror ("pipe()");
          exit (2);
        }
      gchar* fd = g_strdup_printf ("--GTestLogFD=%u", pipes[1]);
      gchar* working_folder = g_file_get_path (parent);
      gchar* argv[] = { /* actually is "gchar const* argv[]" but g_spawn_async() requires a "gchar**" */
              NULL,
              "-q",
              "-l",
              fd,
              NULL
      };
      GPid  pid = 0;
      GError* error = NULL;

      gtk_window_set_title (window, title);
      g_free (title);

      /* FIXME: should only be necessary on UNIX */
      argv[0] = g_strdup_printf ("./%s", base);
      g_free (base);
      base = argv[0];

      if (!g_spawn_async (working_folder, argv, NULL,
                          G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_LEAVE_DESCRIPTORS_OPEN | G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                          NULL, NULL, &pid, &error))
        {
          g_warning ("error executing \"%s\": %s",
                     base, error->message);
          g_error_free (error);
          gtk_widget_set_sensitive (button_run, FALSE);
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
          gtk_widget_set_sensitive (button_run, TRUE);

          gtk_progress_bar_pulse (GTK_PROGRESS_BAR (progress));
        }
      close (pipes[1]);

      g_free (fd);
      g_free (working_folder);
      g_free (base);
    }

  if (!selected)
    {
      gtk_window_set_title (window, _("GLib Unit Tests"));
      gtk_widget_set_sensitive (button_run, FALSE);
    }
  else
    {
      g_object_unref (selected);
    }

  gtk_widget_set_sensitive (notebook, selected != NULL);
}

int
main (int   argc,
      char**argv)
{
  GtkWidget* box;
  GtkWidget* file_chooser;
  GtkWidget* scrolled;
  GtkWidget* tree;
  GtkWidget* window;

  gtk_init (&argc, &argv);

  map = g_hash_table_new (g_str_hash, g_str_equal);

  store = gtk_tree_store_new (N_COLUMNS, G_TYPE_STRING);

  box = gtk_vbox_new (FALSE, 0);
  button_run = gtk_button_new_from_stock (GTK_STOCK_EXECUTE);
  file_chooser = gtk_file_chooser_button_new (_("Choose Unit Tests"), GTK_FILE_CHOOSER_ACTION_OPEN);
  notebook = gtk_notebook_new ();
  progress = gtk_progress_bar_new ();
  scrolled = gtk_scrolled_window_new (NULL, NULL);
  tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_default_size (GTK_WINDOW (window), 300, 400);
  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  g_signal_connect (file_chooser, "selection-changed",
                    G_CALLBACK (selection_changed_cb), window);
  selection_changed_cb (GTK_FILE_CHOOSER (file_chooser), GTK_WINDOW (window));

  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (tree), -1,
                                              NULL, gtk_cell_renderer_text_new (),
                                              "text", COL_NAME,
                                              NULL);

  gtk_widget_show (file_chooser);
  gtk_box_pack_start (GTK_BOX (box), file_chooser, FALSE, FALSE, 0);
  gtk_widget_show (button_run);
  gtk_box_pack_start (GTK_BOX (box), button_run, FALSE, FALSE, 0);
  /* FIXME: add progress bar */
  /* FIXME: add state information: "Runs: 3/3" "Errors: 2" "Failures: 2" */
#if 0
  gtk_container_add_with_properties (GTK_CONTAINER (notebook), gtk_label_new ("FAILURES"),
                                     "tab-label", _("Failures"),
                                     NULL);
#endif
  gtk_widget_show (tree);
  gtk_container_add (GTK_CONTAINER (scrolled), tree);
  gtk_widget_show (scrolled);
  gtk_container_add_with_properties (GTK_CONTAINER (notebook), scrolled,
                                     "tab-label", _("Hierarchy"),
                                     NULL);
  gtk_widget_show_all (notebook);
  /* FIXME: pack the notebook into a paned with the text view */
  gtk_container_add (GTK_CONTAINER (box), notebook);
  gtk_widget_show (box);
  gtk_container_add (GTK_CONTAINER (window), box);

  gtk_widget_show (window);

  gtk_main ();

  g_hash_table_destroy (map);
  return 0;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
