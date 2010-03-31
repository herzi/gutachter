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
#include <gutachter-hierarchy.h>

G_BEGIN_DECLS

typedef struct _GtkTestSuite        GtkTestSuite;
typedef struct _GtkTestSuiteClass   GtkTestSuiteClass;
typedef struct _GtkTestSuitePrivate GtkTestSuitePrivate;

typedef enum /*< skip >*/
{
  MODE_LIST,
  MODE_TEST
} GtkTestSuiteRunningMode;

typedef enum
{
  GUTACHTER_SUITE_INDETERMINED,
  GUTACHTER_SUITE_LOADING,
  GUTACHTER_SUITE_LOADED,
  GUTACHTER_SUITE_RUNNING,
  GUTACHTER_SUITE_FINISHED
} GutachterSuiteStatus;

#define GTK_TEST_TYPE_SUITE         (gtk_test_suite_get_type ())
#define GTK_TEST_SUITE(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), GTK_TEST_TYPE_SUITE, GtkTestSuite))
#define GTK_TEST_SUITE_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), GTK_TEST_TYPE_SUITE, GtkTestSuiteClass))
#define GTK_TEST_IS_SUITE(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), GTK_TEST_TYPE_SUITE))
#define GTK_TEST_IS_SUITE_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), GTK_TEST_TYPE_SUITE))
#define GTK_TEST_SUITE_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), GTK_TEST_TYPE_SUITE, GtkTestSuiteClass))

GType                 gtk_test_suite_get_type       (void);
GtkTestSuite*         gtk_test_suite_new            (GFile       * file);
GTestLogBuffer*       gtk_test_suite_get_buffer     (GtkTestSuite* self);
GIOChannel*           gtk_test_suite_get_channel    (GtkTestSuite* self);
guint64               gtk_test_suite_get_executed   (GtkTestSuite* self);
guint64               gtk_test_suite_get_failures   (GtkTestSuite* self);
GFile*                gtk_test_suite_get_file       (GtkTestSuite* self);
gboolean              gtk_test_suite_get_passed     (GtkTestSuite* self);
GutachterSuiteStatus  gtk_test_suite_get_status     (GtkTestSuite* self);
guint64               gtk_test_suite_get_tests      (GtkTestSuite* self);
GtkTreeModel*         gtk_test_suite_get_tree       (GtkTestSuite* self);
gboolean              io_func                       (GIOChannel  * channel,
                                                     GIOCondition  condition,
                                                     gpointer      data);
void                run_test_child_watch          (GPid      pid,
                                                   gint      status,
                                                   gpointer  user_data);
void                child_watch_cb                (GPid      pid,
                                                   gint      status,
                                                   gpointer  data);
gboolean            run_or_warn                   (GPid                   * pid,
                                                   guint                    pipe_id,
                                                   GtkTestSuiteRunningMode  mode,
                                                   GtkTestSuite           * self);
void                gtk_test_suite_load           (GtkTestSuite           * self);
void                gtk_test_suite_read_available (GtkTestSuite           * self);
void                gtk_test_suite_reset          (GtkTestSuite           * self);
void                gtk_test_suite_set_channel    (GtkTestSuite           * self,
                                                   GIOChannel             * channel);
void                gtk_test_suite_set_executed   (GtkTestSuite           * self,
                                                   guint64                  executed);
void                gtk_test_suite_set_status     (GtkTestSuite           * self,
                                                   GutachterSuiteStatus     status);
void                gtk_test_suite_set_tests    (GtkTestSuite      * self,
                                                 guint64             tests);

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
