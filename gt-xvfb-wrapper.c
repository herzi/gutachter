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

struct _GtkTestXvfbWrapperPrivate
{
  guint64  display;
};

#define PRIV(i) (((GtkTestXvfbWrapper*)(i))->_private)

G_DEFINE_TYPE (GtkTestXvfbWrapper, gtk_test_xvfb_wrapper, G_TYPE_OBJECT);

static void
gtk_test_xvfb_wrapper_init (GtkTestXvfbWrapper* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GTK_TEST_TYPE_XVFB_WRAPPER, GtkTestXvfbWrapperPrivate);
}

static void
gtk_test_xvfb_wrapper_class_init (GtkTestXvfbWrapperClass* self_class)
{
  g_type_class_add_private (self_class, sizeof (GtkTestXvfbWrapperPrivate));
}

guint64
gtk_test_xvfb_wrapper_get_display (GtkTestXvfbWrapper* self)
{
  g_return_val_if_fail (GTK_TEST_IS_XVFB_WRAPPER (self), G_GUINT64_CONSTANT (0));

  return PRIV (self)->display;
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

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
