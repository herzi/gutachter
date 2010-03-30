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

#ifndef GT_RUNNER_H
#define GT_RUNNER_H

#include <gio/gio.h>
#include <gutachter-suite.h>

G_BEGIN_DECLS

typedef struct _GtkTestRunner      GtkTestRunner;
typedef struct _GtkTestRunnerIface GtkTestRunnerIface;

#define GTK_TEST_TYPE_RUNNER         (gtk_test_runner_get_type ())
#define GTK_TEST_RUNNER(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), GTK_TEST_TYPE_RUNNER, GtkTestRunner))
#define GTK_TEST_IS_RUNNER(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), GTK_TEST_TYPE_RUNNER))
#define GTK_TEST_RUNNER_GET_IFACE(i) (G_TYPE_INSTANCE_GET_INTERFACE ((i), GTK_TEST_TYPE_RUNNER, GtkTestRunnerIface))

GType         gtk_test_runner_get_type  (void);
GFile*        gtk_test_runner_get_file  (GtkTestRunner* self);
GtkTestSuite* gtk_test_runner_get_suite (GtkTestRunner* self);
void          gtk_test_runner_set_file  (GtkTestRunner* self,
                                         GFile        * file);

struct _GtkTestRunnerIface
{
  GTypeInterface  base_interface;

  /* vtable */
  GFile*        (*get_file)  (GtkTestRunner* self);
  GtkTestSuite* (*get_suite) (GtkTestRunner* self);
  void          (*set_file)  (GtkTestRunner* self,
                              GFile        * file);
};

G_END_DECLS

#endif /* !GT_RUNNER_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
