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

#include "gt-suite.h"

#include <gtk/gtk.h>
#include <gtk-test.h>

struct _GtkTestSuitePrivate
{
  GTestLogBuffer      * buffer;
  guint64               executed;
  GFile               * file;
  GFileMonitor        * file_monitor;
  GHashTable          * iter_map;
  GutachterSuiteStatus  status;
  guint64               tests;
  GtkTreeStore        * tree_model;
};

enum
{
  PROP_0,
  PROP_FILE,
  PROP_STATUS
};

#define PRIV(i) (((GtkTestSuite*)(i))->_private)

G_DEFINE_TYPE (GtkTestSuite, gtk_test_suite, G_TYPE_OBJECT);

static void
gtk_test_suite_init (GtkTestSuite* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GTK_TEST_TYPE_SUITE, GtkTestSuitePrivate);

  PRIV (self)->buffer = g_test_log_buffer_new ();
  PRIV (self)->iter_map = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GFreeFunc)gtk_tree_row_reference_free);
  PRIV (self)->tree_model = gtk_test_hierarchy_new ();
}

static void
finalize (GObject* object)
{
  g_object_unref (PRIV (object)->tree_model);
  g_object_unref (PRIV (object)->file_monitor);
  g_object_unref (PRIV (object)->file);
  g_test_log_buffer_free (PRIV (object)->buffer);
  g_hash_table_destroy (PRIV (object)->iter_map);

  G_OBJECT_CLASS (gtk_test_suite_parent_class)->finalize (object);
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
get_property (GObject   * object,
              guint       prop_id,
              GValue    * value,
              GParamSpec* pspec)
{
  switch (prop_id)
    {
    case PROP_STATUS:
      g_value_set_enum (value, PRIV (object)->status);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
set_property (GObject     * object,
              guint         prop_id,
              GValue const* value,
              GParamSpec  * pspec)
{
  GError* error = NULL;

  switch (prop_id)
    {
    case PROP_FILE:
      g_return_if_fail (!PRIV (object)->file);
      g_return_if_fail (g_value_get_object (value));
      PRIV (object)->file = g_value_dup_object (value);

      PRIV (object)->file_monitor = g_file_monitor (PRIV (object)->file, G_FILE_MONITOR_NONE, NULL, &error);
      g_signal_connect (PRIV (object)->file_monitor, "changed",
                        G_CALLBACK (file_changed_cb), object);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_test_suite_class_init (GtkTestSuiteClass* self_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS (self_class);

  object_class->finalize     = finalize;
  object_class->get_property = get_property;
  object_class->set_property = set_property;

  g_object_class_install_property (object_class, PROP_FILE,
                                   g_param_spec_object ("file", NULL, NULL,
                                                        G_TYPE_FILE, G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
  g_object_class_install_property (object_class, PROP_STATUS,
                                   g_param_spec_enum ("status", NULL, NULL,
                                                      GUTACHTER_TYPE_SUITE_STATUS,
                                                      GUTACHTER_SUITE_INDETERMINED,
                                                      G_PARAM_READABLE));

  g_type_class_add_private (self_class, sizeof (GtkTestSuitePrivate));
}

gboolean
lookup_iter_for_path (GtkTestSuite* suite,
                      GtkTreeIter * iter,
                      gchar       * path)
{
  GtkTreeRowReference* reference = g_hash_table_lookup (gtk_test_suite_get_iter_map (suite), path);
  if (reference)
    {
      GtkTreeStore* store = GTK_TREE_STORE (gtk_test_suite_get_tree (suite));
      GtkTreePath* tree_path = gtk_tree_row_reference_get_path (reference);
      g_assert (gtk_tree_model_get_iter (GTK_TREE_MODEL (store), iter, tree_path));
      gtk_tree_path_free (tree_path);
      return TRUE;
    }

  return FALSE;
}

void
create_iter_for_path (GtkTestSuite* self,
                      GtkTreeIter * iter,
                      gchar       * path)
{
  GtkTreeRowReference* reference;
  GtkTreeStore       * store = GTK_TREE_STORE (gtk_test_suite_get_tree (self));
  GtkTreePath        * tree_path;
  gchar              * last_slash;

  if (lookup_iter_for_path (self, iter, path))
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
      create_iter_for_path (self, &parent, g_strdup (path));
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
  g_hash_table_insert (gtk_test_suite_get_iter_map (self), path, reference);
  gtk_tree_path_free (tree_path);
}

GTestLogBuffer*
gtk_test_suite_get_buffer (GtkTestSuite* self)
{
  g_return_val_if_fail (GTK_TEST_IS_SUITE (self), NULL);

  return PRIV (self)->buffer;
}

guint64
gtk_test_suite_get_executed (GtkTestSuite* self)
{
  g_return_val_if_fail (GTK_TEST_IS_SUITE (self), G_GUINT64_CONSTANT (0));

  return PRIV (self)->executed;
}

GFile*
gtk_test_suite_get_file (GtkTestSuite* self)
{
  g_return_val_if_fail (GTK_TEST_IS_SUITE (self), NULL);

  return PRIV (self)->file;
}

GHashTable*
gtk_test_suite_get_iter_map (GtkTestSuite* self)
{
  g_return_val_if_fail (GTK_TEST_IS_SUITE (self), NULL);

  return PRIV (self)->iter_map;
}

GutachterSuiteStatus
gtk_test_suite_get_status (GtkTestSuite* self)
{
  g_return_val_if_fail (GTK_TEST_IS_SUITE (self), GUTACHTER_SUITE_INDETERMINED);

  return PRIV (self)->status;
}

guint64
gtk_test_suite_get_tests (GtkTestSuite* self)
{
  g_return_val_if_fail (GTK_TEST_IS_SUITE (self), G_GUINT64_CONSTANT (0));

  return PRIV (self)->tests;
}

GtkTreeModel*
gtk_test_suite_get_tree (GtkTestSuite* self)
{
  g_return_val_if_fail (GTK_TEST_IS_SUITE (self), G_GUINT64_CONSTANT (0));

  return GTK_TREE_MODEL (PRIV (self)->tree_model);
}

GtkTestSuite*
gtk_test_suite_new (GFile* file)
{
  return g_object_new (GTK_TEST_TYPE_SUITE,
                       "file", file,
                       NULL);
}

gboolean
run_or_warn (GPid                   * pid,
             guint                    pipe_id,
             GtkTestSuiteRunningMode  mode,
             GtkTestSuite           * self)
{
  GtkTestXvfbWrapper* xvfb = gtk_test_xvfb_wrapper_get_instance ();
  gboolean  result = FALSE;
  GError  * error  = NULL;
  GFile   * parent = g_file_get_parent (PRIV (self)->file);
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

  base = g_file_get_basename (PRIV (self)->file);

  /* FIXME: this is X11 specific */
  for (iter = env; iter && *iter; iter++)
    {
      if (!g_str_has_prefix (*iter, "DISPLAY="))
        {
          g_free (*iter);
          *iter = g_strdup_printf ("DISPLAY=:%" G_GUINT64_FORMAT,
                                   gtk_test_xvfb_wrapper_get_display (xvfb));
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
  g_object_unref (xvfb);

  return result;
}


void
gtk_test_suite_reset (GtkTestSuite* self)
{
  g_return_if_fail (GTK_TEST_IS_SUITE (self));

  PRIV (self)->tests = G_GUINT64_CONSTANT (0);
  g_hash_table_remove_all (PRIV (self)->iter_map);
  gtk_tree_store_clear (PRIV (self)->tree_model);
}

void
gtk_test_suite_set_executed (GtkTestSuite* self,
                             guint64       executed)
{
  g_return_if_fail (GTK_TEST_IS_SUITE (self));

  PRIV (self)->executed = executed;
}

void
gtk_test_suite_set_status (GtkTestSuite      * self,
                           GutachterSuiteStatus  status)
{
  g_return_if_fail (GTK_TEST_IS_SUITE (self));
  /* comparison of unsigned int with 0 is always true; FIXME: is there a way to glue this into a test case? */
  g_return_if_fail (/*GTK_TEST_SUITE_INDETERMINED <= status && */ status <= GUTACHTER_SUITE_FINISHED);

  if (PRIV (self)->status == status)
    {
      return;
    }

  /* cannot switch from any status to any other */
  switch (status)
    {
    case GUTACHTER_SUITE_LOADING:
      g_return_if_fail (PRIV (self)->status == GUTACHTER_SUITE_INDETERMINED);
      break;
    case GUTACHTER_SUITE_LOADED:
      g_return_if_fail (PRIV (self)->status == GUTACHTER_SUITE_LOADING);
      break;
    case GUTACHTER_SUITE_RUNNING:
      g_return_if_fail (PRIV (self)->status >= GUTACHTER_SUITE_LOADED);
      break;
    case GUTACHTER_SUITE_FINISHED:
      g_return_if_fail (PRIV (self)->status == GUTACHTER_SUITE_RUNNING);
      break;
    default:
      g_warning ("%s(%s): unexpected status: %d",
                 G_STRFUNC, G_STRLOC, status);
      g_return_if_reached ();
      break;
    }

  PRIV (self)->status = status;
  g_object_notify (G_OBJECT (self), "status");
}

void
gtk_test_suite_set_tests (GtkTestSuite* self,
                          guint64       tests)
{
  g_return_if_fail (GTK_TEST_IS_SUITE (self));

  PRIV (self)->tests = tests;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
