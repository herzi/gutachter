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

G_DEFINE_TYPE (GtkTestSuite, gtk_test_suite, G_TYPE_OBJECT);

static void
gtk_test_suite_init (GtkTestSuite* self G_GNUC_UNUSED)
{}

static void
gtk_test_suite_class_init (GtkTestSuiteClass* self_class G_GNUC_UNUSED)
{}

GtkTestSuite*
gtk_test_suite_new (GFile* file G_GNUC_UNUSED)
{
  return g_object_new (GTK_TEST_TYPE_SUITE,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
