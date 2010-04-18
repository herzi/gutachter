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

#include "gutachter-xvfb.h"

#include <errno.h>    /* errno */
#include <string.h>   /* strerror() */
#include <sys/wait.h> /* WIFEXITED() */

/* FIXME: make sure we kill Xvfb when a signal handler gets triggered */

struct _GutachterXvfbPrivate
{
  guint64  display;
  GPid     pid;
  gulong   child_watch;
  gulong   idle_id;
};

#define PRIV(i) (((GutachterXvfb*)(i))->_private)

static gboolean setup_xvfb (gpointer  data);

static GObject* instance = NULL;

G_DEFINE_TYPE (GutachterXvfb, gutachter_xvfb, G_TYPE_OBJECT);

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
          PRIV (user_data)->idle_id = g_idle_add (setup_xvfb, user_data);
        }
    }
  else if (WIFSIGNALED (status))
    {
    }

  g_assert_cmpint (pid, ==, PRIV (user_data)->pid);

  PRIV (user_data)->pid = 0;
  PRIV (user_data)->child_watch = 0;

  g_object_unref (user_data);
}

static gboolean
setup_xvfb (gpointer data)
{
  GutachterXvfb* self = data;
  gchar* display;
  gchar* argv[] = {
          "Xvfb",
          NULL,
          NULL
  };
  GError* error = NULL;

  g_return_val_if_fail (GUTACHTER_IS_XVFB (self), FALSE);

  g_assert_cmpint (PRIV (self)->pid, ==, 0);

  display = g_strdup_printf (":%" G_GUINT64_FORMAT, ++PRIV (self)->display);
  argv[1] = display;

  if (g_spawn_async (g_get_home_dir (),
                     argv, NULL,
                     G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                     NULL, NULL,
                     &PRIV (self)->pid, &error))
    {
      PRIV (self)->child_watch = g_child_watch_add (PRIV (self)->pid, xvfb_child_watch, g_object_ref (self));
    }
  else
    {
      g_warning ("error starting Xvfb: %s", error->message);
      g_error_free (error);
    }

  g_free (display);
  g_object_unref (self);
  return FALSE;
}

static void
gutachter_xvfb_init (GutachterXvfb* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GUTACHTER_TYPE_XVFB, GutachterXvfbPrivate);
}

static GObject*
constructor (GType                  type,
             guint                  n_params,
             GObjectConstructParam* params)
{
  if (G_UNLIKELY (!instance))
    {
      instance = G_OBJECT_CLASS (gutachter_xvfb_parent_class)->constructor (type, n_params, params);

      PRIV (instance)->idle_id = g_idle_add (setup_xvfb, g_object_ref (instance));
    }
  else
    {
      g_object_ref (instance);
    }

  return instance;
}

static void
finalize_child_watch (GPid      pid,
                      gint      status    G_GNUC_UNUSED,
                      gpointer  user_data G_GNUC_UNUSED)
{
  g_spawn_close_pid (pid);
}

static void
finalize (GObject* object)
{
  if (PRIV (object)->idle_id)
    {
      g_source_remove (PRIV (object)->idle_id);
    }

  if (PRIV (object)->child_watch)
    {
      g_source_remove (PRIV (object)->child_watch);
      PRIV (object)->child_watch = g_child_watch_add (PRIV (object)->pid, finalize_child_watch, NULL);
    }

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

  instance = NULL;

  G_OBJECT_CLASS (gutachter_xvfb_parent_class)->finalize (object);
}

static void
gutachter_xvfb_class_init (GutachterXvfbClass* self_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS (self_class);

  object_class->constructor = constructor;
  object_class->finalize    = finalize;

  g_type_class_add_private (self_class, sizeof (GutachterXvfbPrivate));
}

guint64
gutachter_xvfb_get_display (GutachterXvfb* self)
{
  g_return_val_if_fail (GUTACHTER_IS_XVFB (self), G_GUINT64_CONSTANT (0));

  return PRIV (self)->display;
}

GPid
gutachter_xvfb_get_pid (GutachterXvfb* self)
{
  g_return_val_if_fail (GUTACHTER_IS_XVFB (self), 0);

  return PRIV (self)->pid;
}

GutachterXvfb*
gutachter_xvfb_get_instance (void)
{
  return g_object_new (GUTACHTER_TYPE_XVFB,
                       NULL);
}

void
gutachter_xvfb_set_display (GutachterXvfb* self,
                            guint64        display)
{
  g_return_if_fail (GUTACHTER_IS_XVFB (self));

  PRIV (self)->display = display;
}

void
gutachter_xvfb_set_pid (GutachterXvfb* self,
                        GPid           pid)
{
  g_return_if_fail (GUTACHTER_IS_XVFB (self));

  PRIV (self)->pid = pid;
}

gboolean
gutachter_xvfb_wait (GutachterXvfb* self,
                     GError       **error)
{
  g_return_val_if_fail (GUTACHTER_IS_XVFB (self), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  while (!gutachter_xvfb_get_pid (self))
    {
      g_main_context_iteration (NULL, FALSE);
    }

  return TRUE;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
