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

#include <errno.h>  /* errno */
#include <string.h> /* strerror() */

struct _GtkTestXvfbWrapperPrivate
{
  guint64  display;
  GPid     pid;
};

#define PRIV(i) (((GtkTestXvfbWrapper*)(i))->_private)

G_DEFINE_TYPE (GtkTestXvfbWrapper, gtk_test_xvfb_wrapper, G_TYPE_OBJECT);

static void
gtk_test_xvfb_wrapper_init (GtkTestXvfbWrapper* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GTK_TEST_TYPE_XVFB_WRAPPER, GtkTestXvfbWrapperPrivate);
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
