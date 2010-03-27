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

struct _GtkTestSuitePrivate
{
  guint64     executed;
  GHashTable* iter_map;
  guint64     tests;
};

#define PRIV(i) (((GtkTestSuite*)(i))->_private)

G_DEFINE_TYPE (GtkTestSuite, gtk_test_suite, G_TYPE_OBJECT);

static void
gtk_test_suite_init (GtkTestSuite* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GTK_TEST_TYPE_SUITE, GtkTestSuitePrivate);

  PRIV (self)->iter_map = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GFreeFunc)gtk_tree_row_reference_free);
}

static void
finalize (GObject* object)
{
  g_hash_table_destroy (PRIV (object)->iter_map);

  G_OBJECT_CLASS (gtk_test_suite_parent_class)->finalize (object);
}

static void
gtk_test_suite_class_init (GtkTestSuiteClass* self_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS (self_class);

  object_class->finalize = finalize;

  g_type_class_add_private (self_class, sizeof (GtkTestSuitePrivate));
}

guint64
gtk_test_suite_get_executed (GtkTestSuite* self)
{
  g_return_val_if_fail (GTK_TEST_IS_SUITE (self), G_GUINT64_CONSTANT (0));

  return PRIV (self)->executed;
}

GHashTable*
gtk_test_suite_get_iter_map (GtkTestSuite* self)
{
  g_return_val_if_fail (GTK_TEST_IS_SUITE (self), NULL);

  return PRIV (self)->iter_map;
}

guint64
gtk_test_suite_get_tests (GtkTestSuite* self)
{
  g_return_val_if_fail (GTK_TEST_IS_SUITE (self), G_GUINT64_CONSTANT (0));

  return PRIV (self)->tests;
}

GtkTestSuite*
gtk_test_suite_new (GFile* file G_GNUC_UNUSED)
{
  return g_object_new (GTK_TEST_TYPE_SUITE,
                       NULL);
}

void
gtk_test_suite_set_executed (GtkTestSuite* self,
                             guint64       executed)
{
  g_return_if_fail (GTK_TEST_IS_SUITE (self));

  PRIV (self)->executed = executed;
}

void
gtk_test_suite_set_tests (GtkTestSuite* self,
                          guint64       tests)
{
  g_return_if_fail (GTK_TEST_IS_SUITE (self));

  PRIV (self)->tests = tests;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
