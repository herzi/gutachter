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

#ifndef GT_SUITE_H
#define GT_SUITE_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GtkTestSuite        GtkTestSuite;
typedef struct _GtkTestSuiteClass   GtkTestSuiteClass;
typedef struct _GtkTestSuitePrivate GtkTestSuitePrivate;

typedef enum
{
  MODE_LIST,
  MODE_TEST
} GtkTestSuiteRunningMode;

#define GTK_TEST_TYPE_SUITE         (gtk_test_suite_get_type ())
#define GTK_TEST_SUITE(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), GTK_TEST_TYPE_SUITE, GtkTestSuite))
#define GTK_TEST_SUITE_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), GTK_TEST_TYPE_SUITE, GtkTestSuiteClass))
#define GTK_TEST_IS_SUITE(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), GTK_TEST_TYPE_SUITE))
#define GTK_TEST_IS_SUITE_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), GTK_TEST_TYPE_SUITE))
#define GTK_TEST_SUITE_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), GTK_TEST_TYPE_SUITE, GtkTestSuiteClass))

GType         gtk_test_suite_get_type     (void);
GtkTestSuite* gtk_test_suite_new          (GFile       * file);
GByteArray*   gtk_test_suite_get_buffer   (GtkTestSuite* self);
guint64       gtk_test_suite_get_executed (GtkTestSuite* self);
GFile*        gtk_test_suite_get_file     (GtkTestSuite* self);
GHashTable*   gtk_test_suite_get_iter_map (GtkTestSuite* self);
guint64       gtk_test_suite_get_tests    (GtkTestSuite* self);
GtkTreeModel* gtk_test_suite_get_tree     (GtkTestSuite* self);
gboolean      run_or_warn                 (GPid                   * pid,
                                           guint                    pipe_id,
                                           GtkTestSuiteRunningMode  mode,
                                           GtkTestSuite           * self);
void          gtk_test_suite_set_executed (GtkTestSuite* self,
                                           guint64       executed);
void          gtk_test_suite_set_tests    (GtkTestSuite* self,
                                           guint64       tests);

struct _GtkTestSuite
{
  GObject              base_instance;
  GtkTestSuitePrivate* _private;
};

struct _GtkTestSuiteClass
{
  GObjectClass         base_class;
};

G_END_DECLS

#endif /* !GT_SUITE_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
