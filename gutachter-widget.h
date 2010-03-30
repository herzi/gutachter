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

#ifndef GT_WIDGET_H
#define GT_WIDGET_H

#include <gtk/gtk.h>
#include <gt-suite.h>

G_BEGIN_DECLS

typedef struct _GtkTestWidget        GtkTestWidget;
typedef struct _GtkTestWidgetClass   GtkTestWidgetClass;
typedef struct _GtkTestWidgetPrivate GtkTestWidgetPrivate;

#define GTK_TEST_TYPE_WIDGET         (gtk_test_widget_get_type ())
#define GTK_TEST_WIDGET(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), GTK_TEST_TYPE_WIDGET, GtkTestWidget))
#define GTK_TEST_WIDGET_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), GTK_TEST_TYPE_WIDGET, GtkTestWidgetClass))
#define GTK_TEST_IS_WIDGET(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), GTK_TEST_TYPE_WIDGET))
#define GTK_TEST_IS_WIDGET_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), GTK_TEST_TYPE_WIDGET))
#define GTK_TEST_WIDGET_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), GTK_TEST_TYPE_WIDGET, GtkTestWidgetClass))

GType      gtk_test_widget_get_type      (void);
GtkWidget* gtk_test_widget_new           (void);
#ifndef REFACTORING_FINISHED
GtkWidget* gtk_test_widget_get_hierarchy (GtkTestWidget* self);
GtkWidget* gtk_test_widget_get_notebook  (GtkTestWidget* self);
GtkWidget* gtk_test_widget_get_progress  (GtkTestWidget* self);
#endif
void       gtk_test_widget_set_suite     (GtkTestWidget* self,
                                          GtkTestSuite * suite); /* FIXME: consider moving to GtkTestRunner */

struct _GtkTestWidget
{
  GtkVBox               base_instance;
  GtkTestWidgetPrivate* _private;
};

struct _GtkTestWidgetClass
{
  GtkVBoxClass          base_class;
};

G_END_DECLS

#endif /* !GT_WIDGET_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
