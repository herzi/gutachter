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

#include "gutachter-suite.h"

#include <sys/wait.h> /* WIFEXITED() */
#include <unistd.h>   /* pipe() */
#include <gtk/gtk.h>
#include <gutachter.h>

#include <glib/gi18n.h>

/**
 * GutachterSuite
 *
 * A representation for a test binary.
 */
/**
 * GutachterSuiteStatus:
 * @GUTACHTER_SUITE_INDETERMINED: the suite is freshly created
 * @GUTACHTER_SUITE_LOADING: the suite is currently loading the list of tests
 * @GUTACHTER_SUITE_LOADED: the suite is done loading
 * @GUTACHTER_SUITE_RUNNING: the suite is being executed right now
 * @GUTACHTER_SUITE_FINISHED: the suite has been executed and is finished
 *
 * Represents the internal state of the #GutachterSuite.
 */

struct _GutachterSuitePrivate
{
  GTestLogBuffer      * buffer;
  GIOChannel          * channel;
  guint32               child_watch;
  GError              * error;
  guint64               executed;
  guint64               failures;
  GFile               * file;
  GFileMonitor        * file_monitor;
  GutachterHierarchy  * hierarchy;
  guint32               io_watch;
  GtkTreeIter           iter;
  guint                 passed : 1;
  guint                 passes_current_test : 1;
  GPid                  pid;
  GutachterSuiteStatus  status;
  guint64               tests;
};

enum
{
  PROP_0,
  PROP_FILE,
  PROP_STATUS
};

#define PRIV(i) (((GutachterSuite*)(i))->_private)

G_DEFINE_TYPE (GutachterSuite, gutachter_suite, G_TYPE_OBJECT);

static void
gutachter_suite_init (GutachterSuite* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GUTACHTER_TYPE_SUITE, GutachterSuitePrivate);

  PRIV (self)->buffer = g_test_log_buffer_new ();
  PRIV (self)->hierarchy = gutachter_hierarchy_new ();
  PRIV (self)->passed = TRUE;
}

static void
cleanup_func (GPid      pid,
              gint      status G_GNUC_UNUSED,
              gpointer  user_data G_GNUC_UNUSED)
{
  g_spawn_close_pid (pid);
}

static void
finalize (GObject* object)
{
  if (PRIV (object)->error)
    {
      g_error_free (PRIV (object)->error);
    }

  if (PRIV (object)->io_watch)
    {
      g_source_remove (PRIV (object)->io_watch);
    }
  if (PRIV (object)->child_watch)
    {
      g_source_remove (PRIV (object)->child_watch);
      g_child_watch_add (PRIV (object)->pid, cleanup_func, NULL);
    }

  g_object_unref (PRIV (object)->hierarchy);
  g_object_unref (PRIV (object)->file_monitor);
  g_object_unref (PRIV (object)->file);
  g_test_log_buffer_free (PRIV (object)->buffer);

  G_OBJECT_CLASS (gutachter_suite_parent_class)->finalize (object);
}

static void
file_changed_cb (GFileMonitor     * monitor    G_GNUC_UNUSED,
                 GFile            * file,
                 GFile            * other_file G_GNUC_UNUSED,
                 GFileMonitorEvent  event,
                 gpointer           user_data)
{
  if (!g_file_equal (file, PRIV (user_data)->file))
    {
      return;
    }

  switch (event)
    {
    case G_FILE_MONITOR_EVENT_CHANGED:
    case G_FILE_MONITOR_EVENT_CREATED:
    case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
      if (PRIV (user_data)->status == GUTACHTER_SUITE_INDETERMINED)
        {
          gutachter_suite_load (user_data);
        }
      break;
    case G_FILE_MONITOR_EVENT_DELETED:
      gutachter_suite_set_status (user_data, GUTACHTER_SUITE_INDETERMINED);
      gutachter_suite_reset (user_data);
      break;
    case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
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
  GFile * folder;

  switch (prop_id)
    {
    case PROP_FILE:
      g_return_if_fail (!PRIV (object)->file);
      g_return_if_fail (g_value_get_object (value));
      PRIV (object)->file = g_value_dup_object (value);

      folder = g_file_get_parent (PRIV (object)->file);

      PRIV (object)->file_monitor = g_file_monitor_directory (folder, G_FILE_MONITOR_NONE, NULL, &error);
      if (PRIV (object)->file_monitor)
        {
          g_signal_connect (PRIV (object)->file_monitor, "changed",
                            G_CALLBACK (file_changed_cb), object);
        }
      g_object_unref (folder);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gutachter_suite_class_init (GutachterSuiteClass* self_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS (self_class);

  object_class->finalize     = finalize;
  object_class->get_property = get_property;
  object_class->set_property = set_property;

  g_object_class_install_property (object_class, PROP_FILE,
                                   g_param_spec_object ("file", NULL, NULL,
                                                        G_TYPE_FILE, G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
  /**
   * GutachterSuite::status
   *
   * The internal state of the suite. It can be any of the
   * #GutachterSuiteStatus values. You can wait for the status to be
   * %GUTACHTER_SUITE_LOADED and then execute the test suite.
   */
  g_object_class_install_property (object_class, PROP_STATUS,
                                   g_param_spec_enum ("status", NULL, NULL,
                                                      GUTACHTER_TYPE_SUITE_STATUS,
                                                      GUTACHTER_SUITE_INDETERMINED,
                                                      G_PARAM_READABLE));

  g_type_class_add_private (self_class, sizeof (GutachterSuitePrivate));
}

static gboolean
run_or_warn (GPid                     * pid,
             guint                      pipe_id,
             GutachterSuiteRunningMode  mode,
             GutachterSuite           * self)
{
  GutachterXvfb* xvfb;
  gboolean       result = FALSE;
  GError       * error  = NULL;
  GFile        * parent;
  gchar        * base;
  gchar        * folder;
  gchar        * argv[] = {
          NULL,
          NULL,
          "-q",
          NULL,
          NULL
  };
  gchar** env;
  gchar** iter;
  gboolean found_display = FALSE;

  xvfb = gutachter_xvfb_get_instance ();

  if (!gutachter_xvfb_wait (xvfb, NULL))
    {
      g_warning ("error while waiting for Xvfb: %s", error->message);
      g_error_free (error);
      g_object_unref (xvfb);
      return FALSE;
    }

  base = g_file_get_basename (PRIV (self)->file);
  parent = g_file_get_parent (PRIV (self)->file);
  folder = g_file_get_path (parent);
  env = g_listenv ();

  /* FIXME: this is X11 specific */
  for (iter = env; iter && *iter; iter++)
    {
      if (!g_str_has_prefix (*iter, "DISPLAY="))
        {
          g_free (*iter);
          *iter = g_strdup_printf ("DISPLAY=:%" G_GUINT64_FORMAT,
                                   gutachter_xvfb_get_display (xvfb));
          found_display = TRUE;
          break;
        }
    }

  if (!found_display)
    {
      gchar** new_env = g_new (gchar*, g_strv_length (env) + 2);
      gchar** new_iter = new_env;

      *new_iter = g_strdup_printf ("DISPLAY=:%" G_GUINT64_FORMAT, gutachter_xvfb_get_display (xvfb));
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
                          G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_LEAVE_DESCRIPTORS_OPEN |
                          G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                          NULL, NULL, pid, &error);

  if (!result)
    {
      gutachter_suite_set_status (self, GUTACHTER_SUITE_ERROR);

      PRIV (self)->error = error;
      error = NULL;
    }

  g_free (argv[1]);
  g_free (argv[0]);
  g_free (folder);
  g_object_unref (parent);
  g_object_unref (xvfb);

  return result;
}

static gboolean
io_func (GIOChannel  * channel,
         GIOCondition  condition G_GNUC_UNUSED,
         gpointer      user_data)
{
  GutachterSuite* suite = user_data;
  guchar          buf[512];
  gsize           read_bytes = 0;
  GIOStatus       status;

  for (status = g_io_channel_read_chars (channel, (gchar*)buf, sizeof (buf), &read_bytes, NULL);
       status == G_IO_STATUS_NORMAL;
       status = g_io_channel_read_chars (channel, (gchar*)buf, sizeof (buf), &read_bytes, NULL))
    {
      g_test_log_buffer_push (gutachter_suite_get_buffer (suite), read_bytes, buf);
    }

  gutachter_suite_read_available (suite);

  return TRUE;
}

static void
run_test_child_watch (GPid      pid,
                      gint      status,
                      gpointer  user_data)
{
  GutachterSuite* suite = user_data;

  g_spawn_close_pid (pid);

  if (WIFEXITED (status) && WEXITSTATUS (status))
    {
      g_warning (_("exited with exit code %d"), WEXITSTATUS (status));
    }
  else if (WIFSIGNALED (status))
    {
      g_message ("FIXME: exited with signal %d, trigger restart", WTERMSIG (status));
    }
  else if (WIFEXITED (status))
    {
      gutachter_suite_read_available (suite);
    }

  if (PRIV (suite)->io_watch)
    {
      g_source_remove (PRIV (suite)->io_watch);
      PRIV (suite)->io_watch = 0;
    }

  gutachter_suite_read_available (suite);
  gutachter_suite_set_channel (suite, NULL);

  if (gtk_tree_store_iter_is_valid (GTK_TREE_STORE (PRIV (suite)->hierarchy), &PRIV (suite)->iter))
    {
      /* FIXME: move into the hierarchy */
      PRIV (suite)->passed = FALSE;
      PRIV (suite)->failures++;
      gtk_tree_store_set (GTK_TREE_STORE (PRIV (suite)->hierarchy), &PRIV (suite)->iter,
                          GUTACHTER_HIERARCHY_COLUMN_UNSURE, FALSE,
                          GUTACHTER_HIERARCHY_COLUMN_PASSED, FALSE,
                          -1);
    }

  g_source_remove (PRIV (suite)->child_watch);
  PRIV (suite)->child_watch = 0;

  gutachter_suite_set_status (suite, GUTACHTER_SUITE_FINISHED);
}

void
gutachter_suite_execute (GutachterSuite* self)
{
  int            pipes[2];

  g_return_if_fail (GUTACHTER_IS_SUITE (self));

  if (pipe (pipes))
    {
      perror ("pipe()");
      return;
    }

  gutachter_suite_set_executed (self, 0);
  if (!run_or_warn (&PRIV (self)->pid, pipes[1], MODE_TEST, self))
    {
      close (pipes[0]);
    }
  else
    {
      GIOChannel* channel = g_io_channel_unix_new (pipes[0]);
      g_io_channel_set_close_on_unref (channel, TRUE);
      g_io_channel_set_encoding (channel, NULL, NULL);
      g_io_channel_set_buffered (channel, FALSE);
      g_io_channel_set_flags (channel, G_IO_FLAG_NONBLOCK, NULL);
      g_assert (!PRIV (self)->io_watch);
      PRIV (self)->io_watch = g_io_add_watch (channel, G_IO_IN, io_func, self);
      PRIV (self)->child_watch = g_child_watch_add (PRIV (self)->pid, run_test_child_watch, self);

      gutachter_suite_set_status (self, GUTACHTER_SUITE_RUNNING);
      gutachter_suite_set_channel (self, channel);
      g_io_channel_unref (channel);

      gutachter_hierarchy_reset (GUTACHTER_HIERARCHY (gutachter_suite_get_tree (self)));
    }

  close (pipes[1]);
}

GTestLogBuffer*
gutachter_suite_get_buffer (GutachterSuite* self)
{
  g_return_val_if_fail (GUTACHTER_IS_SUITE (self), NULL);

  return PRIV (self)->buffer;
}

GIOChannel*
gutachter_suite_get_channel (GutachterSuite* self)
{
  g_return_val_if_fail (GUTACHTER_IS_SUITE (self), NULL);

  return PRIV (self)->channel;
}

GError*
gutachter_suite_get_error (GutachterSuite* self)
{
  g_return_val_if_fail (GUTACHTER_IS_SUITE (self), NULL);
  g_return_val_if_fail (PRIV (self)->status == GUTACHTER_SUITE_ERROR, NULL);
  g_return_val_if_fail (PRIV (self)->error, NULL);

  return PRIV (self)->error;
}

guint64
gutachter_suite_get_executed (GutachterSuite* self)
{
  g_return_val_if_fail (GUTACHTER_IS_SUITE (self), G_GUINT64_CONSTANT (0));

  return PRIV (self)->executed;
}

guint64
gutachter_suite_get_failures (GutachterSuite* self)
{
  g_return_val_if_fail (GUTACHTER_IS_SUITE (self), G_GUINT64_CONSTANT (0));

  return PRIV (self)->failures;
}

GFile*
gutachter_suite_get_file (GutachterSuite* self)
{
  g_return_val_if_fail (GUTACHTER_IS_SUITE (self), NULL);

  return PRIV (self)->file;
}

gboolean
gutachter_suite_get_passed (GutachterSuite* self)
{
  g_return_val_if_fail (GUTACHTER_IS_SUITE (self), FALSE);

  return PRIV (self)->passed;
}

GutachterSuiteStatus
gutachter_suite_get_status (GutachterSuite* self)
{
  g_return_val_if_fail (GUTACHTER_IS_SUITE (self), GUTACHTER_SUITE_INDETERMINED);

  return PRIV (self)->status;
}

guint64
gutachter_suite_get_tests (GutachterSuite* self)
{
  g_return_val_if_fail (GUTACHTER_IS_SUITE (self), G_GUINT64_CONSTANT (0));

  return PRIV (self)->tests;
}

GtkTreeModel*
gutachter_suite_get_tree (GutachterSuite* self)
{
  g_return_val_if_fail (GUTACHTER_IS_SUITE (self), G_GUINT64_CONSTANT (0));

  return GTK_TREE_MODEL (PRIV (self)->hierarchy);
}

GutachterSuite*
gutachter_suite_new (GFile* file)
{
  return g_object_new (GUTACHTER_TYPE_SUITE,
                       "file", file,
                       NULL);
}

static void
child_watch_cb (GPid      pid,
                gint      status,
                gpointer  data)
{
  GutachterSuite* suite = data;

  g_spawn_close_pid (pid);

  gutachter_suite_read_available (suite);
  gutachter_suite_set_channel (suite, NULL);

  if (PRIV (suite)->io_watch)
    {
      g_source_remove (PRIV (suite)->io_watch);
      PRIV (suite)->io_watch = 0;
    }

  g_source_remove (PRIV (suite)->child_watch);
  PRIV (suite)->child_watch = 0;

  if (WIFEXITED (status) && WEXITSTATUS (status) != 0)
    {
      g_warning ("child exited with error code: %d", WEXITSTATUS (status));
      gutachter_suite_set_status (suite, GUTACHTER_SUITE_INDETERMINED);
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
      gutachter_suite_set_status (suite, GUTACHTER_SUITE_INDETERMINED);
    }
  else
    {
      gutachter_suite_set_status (suite, GUTACHTER_SUITE_LOADED);
    }
}

void
gutachter_suite_load (GutachterSuite* self)
{
  GutachterSuite* suite = self;
  int             pipes[2];

  g_return_if_fail (GUTACHTER_IS_SUITE (self));
  g_return_if_fail (PRIV (self)->status == GUTACHTER_SUITE_INDETERMINED);

  gutachter_suite_reset (suite);

  if (pipe (pipes) < 0)
    {
      perror ("pipe()");
      return;
    }

  if (!run_or_warn (&PRIV (self)->pid, pipes[1], MODE_LIST, suite))
    {
      close (pipes[0]);
    }
  else
    {
      GIOChannel* channel = g_io_channel_unix_new (pipes[0]);
      g_io_channel_set_close_on_unref (channel, TRUE);
      g_io_channel_set_encoding (channel, NULL, NULL);
      g_io_channel_set_buffered (channel, FALSE);
      g_io_channel_set_flags (channel, G_IO_FLAG_NONBLOCK, NULL);
      g_assert (!PRIV (self)->io_watch);
      PRIV (self)->io_watch = g_io_add_watch (channel, G_IO_IN, io_func, suite);
      PRIV (self)->child_watch = g_child_watch_add (PRIV (self)->pid, child_watch_cb, suite);
      gutachter_suite_set_status (suite, GUTACHTER_SUITE_LOADING);
      gutachter_suite_set_channel (suite, channel);
      g_io_channel_unref (channel);
    }
  close (pipes[1]);
}

static void
update_parent (GtkTreeStore* store,
               GtkTreeIter * child)
{
  GtkTreeIter  iter;
  GtkTreeIter  children;
  gboolean     valid;
  gboolean     passed = TRUE;
  gboolean     unsure = FALSE;

  if (!gtk_tree_model_iter_parent (GTK_TREE_MODEL (store), &iter, child))
    {
      return;
    }

  for (valid = gtk_tree_model_iter_children (GTK_TREE_MODEL (store), &children, &iter);
       valid;
       valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &children))
    {
      gboolean this_unsure = FALSE;
      gboolean this_passed = FALSE;

      gtk_tree_model_get (GTK_TREE_MODEL (store), &children,
                          GUTACHTER_HIERARCHY_COLUMN_UNSURE, &this_unsure,
                          GUTACHTER_HIERARCHY_COLUMN_PASSED, &this_passed,
                          -1);

      if (!this_passed)
        {
          unsure = FALSE;
          passed = this_passed;
          break;
        }
      else
        {
          unsure |= this_unsure;
          /* passed stays TRUE */
        }
    }

  gtk_tree_store_set (store, &iter,
                      GUTACHTER_HIERARCHY_COLUMN_UNSURE, unsure,
                      GUTACHTER_HIERARCHY_COLUMN_PASSED, passed,
                      -1);

  update_parent (store, &iter);
}

void
gutachter_suite_read_available (GutachterSuite* self)
{
  GutachterSuiteStatus  status;
  GTestLogBuffer      * tlb; /* FIXME: drop this one */
  GTestLogMsg         * msg;
  GtkTreeStore        * store;

  g_return_if_fail (GUTACHTER_IS_SUITE (self));
  status = gutachter_suite_get_status (self);

  tlb = PRIV (self)->buffer;
  switch (status)
    {
    case GUTACHTER_SUITE_LOADING:
    case GUTACHTER_SUITE_LOADED: /* FIXME: finish the process only after regular EOF */
      for (msg = g_test_log_buffer_pop (tlb); msg; msg = g_test_log_buffer_pop (tlb))
        {
          switch (msg->log_type)
            {
            case G_TEST_LOG_ERROR:
              g_warning ("%s(%s): error while loading test suite: %s",
                         G_STRFUNC, G_STRLOC,
                         msg->strings[0]);
              break;
            case G_TEST_LOG_START_BINARY:
              break;
            case G_TEST_LOG_LIST_CASE:
              gutachter_hierarchy_get_iter (PRIV (self)->hierarchy, &PRIV (self)->iter, msg->strings[0]);
              PRIV (self)->tests++;
              break;
            default:
              g_warning ("%s(%s): unexpected message type: %d",
                         G_STRFUNC, G_STRLOC,
                         msg->log_type);
            }

          g_test_log_msg_free (msg);
        }
      break;
    case GUTACHTER_SUITE_RUNNING:
    case GUTACHTER_SUITE_FINISHED: /* FIXME: finish the process only after regular EOF */
      store = GTK_TREE_STORE (PRIV (self)->hierarchy);
      for (msg = g_test_log_buffer_pop (tlb); msg; msg = g_test_log_buffer_pop (tlb))
        {
          switch (msg->log_type)
            {
            case G_TEST_LOG_START_BINARY:
              break;
            case G_TEST_LOG_START_CASE:
              gutachter_hierarchy_lookup_iter (PRIV (self)->hierarchy, &PRIV (self)->iter, msg->strings[0]);
              PRIV (self)->executed++;
              PRIV (self)->passes_current_test = TRUE;
              break;
            case G_TEST_LOG_STOP_CASE:
              /* FIXME: move into the hierarchy */
              gtk_tree_store_set (store, &PRIV (self)->iter,
                                  GUTACHTER_HIERARCHY_COLUMN_UNSURE, FALSE,
                                  GUTACHTER_HIERARCHY_COLUMN_PASSED, PRIV (self)->passes_current_test && msg->nums[0] == 0,
                                  -1);
              if (!PRIV (self)->passes_current_test)
                {
                  PRIV (self)->passed = FALSE;
                  PRIV (self)->failures++;
                }
              update_parent (store, &PRIV (self)->iter);
#if 0
              g_message ("status %d; nforks %d; elapsed %Lf",
                         (int)msg->nums[0], (int)msg->nums[1], msg->nums[2]);
#endif
              PRIV (self)->iter.stamp = 0;
              PRIV (self)->iter.user_data = NULL;
              PRIV (self)->iter.user_data2 = NULL;
              PRIV (self)->iter.user_data3 = NULL;
              break;
            case G_TEST_LOG_ERROR:
              PRIV (self)->passes_current_test = FALSE;
              /* FIXME: maybe use append_error() -- we could use markup then… */
              gutachter_hierarchy_append_message (PRIV (self)->hierarchy, &PRIV (self)->iter, msg->strings[0]);
              update_parent (store, &PRIV (self)->iter);
              break;
            case G_TEST_LOG_MESSAGE:
              gutachter_hierarchy_append_message (PRIV (self)->hierarchy, &PRIV (self)->iter, msg->strings[0]);
              break;
            default:
              g_warning ("%s(%s): unexpected message type: %d",
                         G_STRFUNC, G_STRLOC,
                         msg->log_type);
            }

          g_test_log_msg_free (msg);
        }
      break;
    default:
      break;
    }
}

void
gutachter_suite_reset (GutachterSuite* self)
{
  g_return_if_fail (GUTACHTER_IS_SUITE (self));

  PRIV (self)->tests = G_GUINT64_CONSTANT (0);
  gutachter_hierarchy_clear (PRIV (self)->hierarchy);
}

void
gutachter_suite_set_channel (GutachterSuite* self,
                             GIOChannel    * channel)
{
  g_return_if_fail (GUTACHTER_IS_SUITE (self));

  if (PRIV (self)->channel == channel)
    {
      return;
    }

  if (PRIV (self)->channel)
    {
      g_io_channel_unref (PRIV (self)->channel);
      PRIV (self)->channel = NULL;
    }

  if (channel)
    {
      PRIV (self)->channel = g_io_channel_ref (channel);
    }
}

void
gutachter_suite_set_executed (GutachterSuite* self,
                              guint64         executed)
{
  g_return_if_fail (GUTACHTER_IS_SUITE (self));

  PRIV (self)->executed = executed;
}

void
gutachter_suite_set_status (GutachterSuite      * self,
                            GutachterSuiteStatus  status)
{
  g_return_if_fail (GUTACHTER_IS_SUITE (self));
  /* comparison of unsigned int with 0 is always true; FIXME: is there a way to glue this into a test case? */
  g_return_if_fail (/*GTK_TEST_SUITE_INDETERMINED <= status && */ status <= GUTACHTER_SUITE_ERROR);

  if (PRIV (self)->status == status)
    {
      return;
    }

  if (PRIV (self)->status == GUTACHTER_SUITE_ERROR)
    {
      g_error_free (PRIV (self)->error);
      PRIV (self)->error = NULL;
    }

  /* cannot switch from any status to any other */
  switch (status)
    {
    case GUTACHTER_SUITE_INDETERMINED:
    case GUTACHTER_SUITE_ERROR:
      /* we can fall into these states from every other */
      break;
    case GUTACHTER_SUITE_LOADING:
      g_return_if_fail (PRIV (self)->status == GUTACHTER_SUITE_INDETERMINED);
      break;
    case GUTACHTER_SUITE_LOADED:
      g_return_if_fail (PRIV (self)->status == GUTACHTER_SUITE_LOADING);
      break;
    case GUTACHTER_SUITE_RUNNING:
      g_return_if_fail (PRIV (self)->status >= GUTACHTER_SUITE_LOADED);
      PRIV (self)->passed = TRUE;
      PRIV (self)->failures = 0;
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

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
