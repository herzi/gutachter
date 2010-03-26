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

#ifndef GG_WINDOW_H
#define GG_WINDOW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GtkTestWindow        GtkTestWindow;
typedef struct _GtkTestWindowClass   GtkTestWindowClass;
typedef struct _GtkTestWindowPrivate GtkTestWindowPrivate;

#define GTK_TEST_TYPE_WINDOW         (gtk_test_window_get_type ())
#define GTK_TEST_WINDOW(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), GTK_TEST_TYPE_WINDOW, GtkTestWindow))
#define GTK_TEST_WINDOW_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), GTK_TEST_TYPE_WINDOW, GtkTestWindowClass))
#define GTK_TEST_IS_WINDOW(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), GTK_TEST_TYPE_WINDOW))
#define GTK_TEST_IS_WINDOW_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), GTK_TEST_TYPE_WINDOW))
#define GTK_TEST_WINDOW_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), GTK_TEST_TYPE_WINDOW, GtkTestWindowClass))

GType      gtk_test_window_get_type    (void);
GtkWidget* gtk_test_window_new         (void);
#ifndef REFACTORING_FINISHED
GtkWidget* gtk_test_window_get_open    (GtkTestWindow* self);
GtkWidget* gtk_test_window_get_toolbar (GtkTestWindow* self);
GtkWidget* gtk_test_window_get_widget  (GtkTestWindow* self);
#endif

struct _GtkTestWindow
{
  GtkWindow             base_instance;
  GtkTestWindowPrivate* _private;
};

struct _GtkTestWindowClass
{
  GtkWindowClass        base_class;
};

G_END_DECLS

#endif /* !GG_WINDOW_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
