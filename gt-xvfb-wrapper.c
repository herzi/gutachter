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

#include "gt-xvfb-wrapper.h"

#include <errno.h>    /* errno */
#include <string.h>   /* strerror() */
#include <sys/wait.h> /* WIFEXITED() */

struct _GtkTestXvfbWrapperPrivate
{
  guint64  display;
  GPid     pid;
};

#define PRIV(i) (((GtkTestXvfbWrapper*)(i))->_private)

static gboolean setup_xvfb (gpointer  data);

G_DEFINE_TYPE (GtkTestXvfbWrapper, gtk_test_xvfb_wrapper, G_TYPE_OBJECT);

static void
xvfb_child_watch (GPid      pid,
                  gint      status,
                  gpointer  user_data)
{
  g_spawn_close_pid (pid);

  if (WIFEXITED (status))
    {
      if (WEXITSTATUS (status))
        {
          g_message ("xvfb exit code: %d", WEXITSTATUS (status));
          g_idle_add (setup_xvfb, user_data);
        }
    }
  else if (WIFSIGNALED (status))
    {
    }

  g_assert_cmpint (pid, ==, PRIV (user_data)->pid);

  PRIV (user_data)->pid = 0;
}

static gboolean
setup_xvfb (gpointer data)
{
  GtkTestXvfbWrapper* self = data;
  gchar* display;
  gchar* argv[] = {
          "Xvfb",
          NULL,
          NULL
  };
  GError* error = NULL;

  g_assert_cmpint (PRIV (self)->pid, ==, 0);

  display = g_strdup_printf (":%" G_GUINT64_FORMAT, ++PRIV (self)->display);
  argv[1] = display;

  if (g_spawn_async (g_get_home_dir (),
                     argv, NULL,
                     G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                     NULL, NULL,
                     &PRIV (self)->pid, &error))
    {
      g_child_watch_add (PRIV (self)->pid, xvfb_child_watch, self);
    }
  else
    {
      g_warning ("error starting Xvfb: %s", error->message);
      g_error_free (error);
    }

  g_free (display);
  return FALSE;
}

static void
gtk_test_xvfb_wrapper_init (GtkTestXvfbWrapper* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GTK_TEST_TYPE_XVFB_WRAPPER, GtkTestXvfbWrapperPrivate);

  g_idle_add (setup_xvfb, self);
}

static void
finalize (GObject* object)
{
  if (PRIV (object)->pid)
    {
      if (kill (PRIV (object)->pid, SIGTERM) < 0)
        {
          int error = errno;
          g_warning ("%s(%s): error killing the Xvfb process (%d on :%" G_GUINT64_FORMAT "): %s",
                     G_STRFUNC, G_STRLOC,
                     PRIV (object)->pid,
                     PRIV (object)->display,
                     strerror (error));
        }
      else
        {
          /* FIXME: try to wait until we are sure that it's quit */
          PRIV (object)->pid = 0;
        }
    }

  G_OBJECT_CLASS (gtk_test_xvfb_wrapper_parent_class)->finalize (object);
}

static void
gtk_test_xvfb_wrapper_class_init (GtkTestXvfbWrapperClass* self_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS (self_class);

  object_class->finalize = finalize;

  g_type_class_add_private (self_class, sizeof (GtkTestXvfbWrapperPrivate));
}

guint64
gtk_test_xvfb_wrapper_get_display (GtkTestXvfbWrapper* self)
{
  g_return_val_if_fail (GTK_TEST_IS_XVFB_WRAPPER (self), G_GUINT64_CONSTANT (0));

  return PRIV (self)->display;
}

GPid
gtk_test_xvfb_wrapper_get_pid (GtkTestXvfbWrapper* self)
{
  g_return_val_if_fail (GTK_TEST_IS_XVFB_WRAPPER (self), 0);

  return PRIV (self)->pid;
}

GtkTestXvfbWrapper*
gtk_test_xvfb_wrapper_new (void)
{
  return g_object_new (GTK_TEST_TYPE_XVFB_WRAPPER,
                       NULL);
}

void
gtk_test_xvfb_wrapper_set_display (GtkTestXvfbWrapper* self,
                                   guint64             display)
{
  g_return_if_fail (GTK_TEST_IS_XVFB_WRAPPER (self));

  PRIV (self)->display = display;
}

void
gtk_test_xvfb_wrapper_set_pid (GtkTestXvfbWrapper* self,
                               GPid                pid)
{
  g_return_if_fail (GTK_TEST_IS_XVFB_WRAPPER (self));

  PRIV (self)->pid = pid;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
