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

#ifndef GT_XVFB_WRAPPER_H
#define GT_XVFB_WRAPPER_H

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _GtkTestXvfbWrapper        GtkTestXvfbWrapper;
typedef struct _GtkTestXvfbWrapperClass   GtkTestXvfbWrapperClass;
typedef struct _GtkTestXvfbWrapperPrivate GtkTestXvfbWrapperPrivate;

#define GTK_TEST_TYPE_XVFB_WRAPPER         (gtk_test_xvfb_wrapper_get_type ())
#define GTK_TEST_XVFB_WRAPPER(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), GTK_TEST_TYPE_XVFB_WRAPPER, GtkTestXvfbWrapper))
#define GTK_TEST_XVFB_WRAPPER_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), GTK_TEST_TYPE_XVFB_WRAPPER, GtkTestXvfbWrapperClass))
#define GTK_TEST_IS_XVFB_WRAPPER(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), GTK_TEST_TYPE_XVFB_WRAPPER))
#define GTK_TEST_IS_XVFB_WRAPPER_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), GTK_TEST_TYPE_XVFB_WRAPPER))
#define GTK_TEST_XVFB_WRAPPER_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), GTK_TEST_TYPE_XVFB_WRAPPER, GtkTestXvfbWrapperClass))

GType               gtk_test_xvfb_wrapper_get_type    (void);
GtkTestXvfbWrapper* gtk_test_xvfb_wrapper_new         (void);
guint64             gtk_test_xvfb_wrapper_get_display (GtkTestXvfbWrapper* self);
void                gtk_test_xvfb_wrapper_set_display (GtkTestXvfbWrapper* self,
                                                       guint64             display);

struct _GtkTestXvfbWrapper
{
  GObject                    base_instance;
  GtkTestXvfbWrapperPrivate* _private;
};

struct _GtkTestXvfbWrapperClass
{
  GObjectClass               base_class;
};

G_END_DECLS

#endif /* !GT_XVFB_WRAPPER_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
