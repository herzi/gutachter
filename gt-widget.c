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

#include "gt-widget.h"

#include <gtk-test.h>
#include <glib/gi18n.h>

struct _GtkTestWidgetPrivate
{
  GtkWidget* hierarchy_view;
  GtkWidget* notebook;
  GtkWidget* progress;
};

#define PRIV(i) (((GtkTestWidget*)(i))->_private)

G_DEFINE_TYPE (GtkTestWidget, gtk_test_widget, GTK_TYPE_VBOX);

static void
gtk_test_widget_init (GtkTestWidget* self)
{
  GtkTreeStore* store;
  GtkWidget* scrolled;

  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GTK_TEST_TYPE_WIDGET, GtkTestWidgetPrivate);
  PRIV (self)->hierarchy_view = gtk_tree_view_new ();
  PRIV (self)->notebook = gtk_notebook_new ();
  PRIV (self)->progress = gtk_progress_bar_new ();

  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (PRIV (self)->hierarchy_view), -1,
                                              NULL, gtk_cell_renderer_text_new (),
                                              "text", GTK_TEST_HIERARCHY_COLUMN_NAME,
                                              NULL);
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (PRIV (self)->hierarchy_view), -1,
                                              NULL, gtk_cell_renderer_toggle_new (),
                                              "active", GTK_TEST_HIERARCHY_COLUMN_PASSED,
                                              NULL);

  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (PRIV (self)->hierarchy_view), FALSE);

  store = gtk_test_hierarchy_new ();
  gtk_tree_view_set_model (GTK_TREE_VIEW (PRIV (self)->hierarchy_view), GTK_TREE_MODEL (store));

  scrolled = gtk_scrolled_window_new (NULL, NULL);

  gtk_container_add (GTK_CONTAINER (scrolled), PRIV (self)->hierarchy_view);
  gtk_widget_show (scrolled);
  gtk_container_add_with_properties (GTK_CONTAINER (PRIV (self)->notebook), scrolled,
                                     "tab-label", _("Hierarchy"),
                                     NULL);
#if 0
  gtk_container_add_with_properties (GTK_CONTAINER (notebook), gtk_label_new ("FAILURES"),
                                     "tab-label", _("Failures"),
                                     NULL);
#endif
}

static void
gtk_test_widget_class_init (GtkTestWidgetClass* self_class)
{
  g_type_class_add_private (self_class, sizeof (GtkTestWidgetPrivate));
}

GtkWidget*
gtk_test_widget_get_hierarchy (GtkTestWidget* self)
{
  g_return_val_if_fail (GTK_TEST_IS_WIDGET (self), NULL);

  return PRIV (self)->hierarchy_view;
}

GtkWidget*
gtk_test_widget_get_notebook (GtkTestWidget* self)
{
  g_return_val_if_fail (GTK_TEST_IS_WIDGET (self), NULL);

  return PRIV (self)->notebook;
}

GtkWidget*
gtk_test_widget_get_progress (GtkTestWidget* self)
{
  g_return_val_if_fail (GTK_TEST_IS_WIDGET (self), NULL);

  return PRIV (self)->progress;
}

GtkWidget*
gtk_test_widget_new (void)
{
  return g_object_new (GTK_TEST_TYPE_WIDGET,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
