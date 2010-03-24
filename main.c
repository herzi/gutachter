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

#include <gtk/gtk.h>

#include <glib/gi18n.h>

static GtkWidget* button_run = NULL;
static GtkWidget* notebook = NULL;

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
      gchar* fd = g_strdup_printf ("%d", 1);
      gchar* working_folder;
      gchar* argv[] = { /* actually is "gchar const* argv[]" but g_spawn_async() requires a "gchar**" */
              base,
              "-l",
              "--GTestLogFD",
              fd,
              NULL
      };
      GPid  pid = 0;
      GError* error = NULL;

      gtk_window_set_title (window, title);
      g_free (title);

      /* FIXME: should only be necessary on UNIX */
      working_folder = base;
      base = g_strdup_printf ("./%s", working_folder);
      g_free (working_folder);
      argv[0] = base;

      working_folder = g_file_get_path (parent);
      if (!g_spawn_async (working_folder, argv, NULL,
                          G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                          NULL, NULL, &pid, &error))
        {
          g_warning ("error executing \"%s\": %s",
                     base, error->message);
          g_error_free (error);
          gtk_widget_set_sensitive (button_run, FALSE);
        }
      else
        {
          gtk_widget_set_sensitive (button_run, TRUE);
        }

      g_free (fd);
      g_free (working_folder);
    }
  else
    {
      gtk_window_set_title (window, _("GLib Unit Tests"));
      gtk_widget_set_sensitive (button_run, FALSE);
    }

  gtk_widget_set_sensitive (notebook, selected != NULL);
}

int
main (int   argc,
      char**argv)
{
  GtkWidget* box;
  GtkWidget* file_chooser;
  GtkWidget* window;

  gtk_init (&argc, &argv);

  box = gtk_vbox_new (FALSE, 0);
  button_run = gtk_button_new_from_stock (GTK_STOCK_EXECUTE);
  file_chooser = gtk_file_chooser_button_new (_("Choose Unit Tests"), GTK_FILE_CHOOSER_ACTION_OPEN);
  notebook = gtk_notebook_new ();
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_default_size (GTK_WINDOW (window), 300, 400);
  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  g_signal_connect (file_chooser, "selection-changed",
                    G_CALLBACK (selection_changed_cb), window);

  selection_changed_cb (GTK_FILE_CHOOSER (file_chooser), GTK_WINDOW (window));

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
  gtk_container_add_with_properties (GTK_CONTAINER (notebook), gtk_label_new ("HIERARCHY"),
                                     "tab-label", _("Hierarchy"),
                                     NULL);
  gtk_widget_show_all (notebook);
  /* FIXME: pack the notebook into a paned with the text view */
  gtk_container_add (GTK_CONTAINER (box), notebook);
  gtk_widget_show (box);
  gtk_container_add (GTK_CONTAINER (window), box);

  gtk_widget_show (window);

  gtk_main ();
  return 0;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
