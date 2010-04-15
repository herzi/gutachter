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

#ifndef GUTACHTER_SUITE_H
#define GUTACHTER_SUITE_H

#include <gtk/gtk.h>
#include <gutachter-hierarchy.h>

G_BEGIN_DECLS

typedef struct _GutachterSuite        GutachterSuite;
typedef struct _GutachterSuiteClass   GutachterSuiteClass;
typedef struct _GutachterSuitePrivate GutachterSuitePrivate;

typedef enum /*< skip >*/ /* FIXME: hide this one */
{
  MODE_LIST,
  MODE_TEST
} GutachterSuiteRunningMode;

typedef enum
{
  GUTACHTER_SUITE_INDETERMINED,
  GUTACHTER_SUITE_LOADING,
  GUTACHTER_SUITE_LOADED,
  GUTACHTER_SUITE_RUNNING,
  GUTACHTER_SUITE_FINISHED,
  GUTACHTER_SUITE_ERROR
} GutachterSuiteStatus;

#define GUTACHTER_TYPE_SUITE         (gutachter_suite_get_type ())
#define GUTACHTER_SUITE(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), GUTACHTER_TYPE_SUITE, GutachterSuite))
#define GUTACHTER_SUITE_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), GUTACHTER_TYPE_SUITE, GutachterSuiteClass))
#define GUTACHTER_IS_SUITE(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), GUTACHTER_TYPE_SUITE))
#define GUTACHTER_IS_SUITE_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), GUTACHTER_TYPE_SUITE))
#define GUTACHTER_SUITE_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), GUTACHTER_TYPE_SUITE, GutachterSuiteClass))

GType                 gutachter_suite_get_type       (void);
GutachterSuite*       gutachter_suite_new            (GFile       * file);
void                  gutachter_suite_execute        (GutachterSuite* self);
GTestLogBuffer*       gutachter_suite_get_buffer     (GutachterSuite* self);
GIOChannel*           gutachter_suite_get_channel    (GutachterSuite* self);
GError*               gutachter_suite_get_error      (GutachterSuite* self);
guint64               gutachter_suite_get_executed   (GutachterSuite* self);
guint64               gutachter_suite_get_failures   (GutachterSuite* self);
GFile*                gutachter_suite_get_file       (GutachterSuite* self);
gboolean              gutachter_suite_get_passed     (GutachterSuite* self);
GutachterSuiteStatus  gutachter_suite_get_status     (GutachterSuite* self);
guint64               gutachter_suite_get_tests      (GutachterSuite* self);
GtkTreeModel*         gutachter_suite_get_tree       (GutachterSuite* self);
gboolean              io_func                        (GIOChannel    * channel,
                                                      GIOCondition    condition,
                                                      gpointer        data);
void                run_test_child_watch             (GPid            pid,
                                                      gint            status,
                                                      gpointer        user_data);
void                child_watch_cb                   (GPid            pid,
                                                      gint            status,
                                                      gpointer        data);
gboolean            run_or_warn                      (GPid                   * pid,
                                                      guint                    pipe_id,
                                                      GutachterSuiteRunningMode  mode,
                                                      GutachterSuite           * self);
void                  gutachter_suite_load           (GutachterSuite* self);
void                  gutachter_suite_read_available (GutachterSuite* self);
void                  gutachter_suite_reset          (GutachterSuite* self);
void                  gutachter_suite_set_channel    (GutachterSuite* self,
                                                      GIOChannel    * channel);
void                  gutachter_suite_set_executed   (GutachterSuite* self,
                                                      guint64         executed);
void                  gutachter_suite_set_status     (GutachterSuite* self,
                                                      GutachterSuiteStatus     status);

struct _GutachterSuite
{
  GObject                base_instance;
  GutachterSuitePrivate* _private;
};

struct _GutachterSuiteClass
{
  GObjectClass         base_class;
};

G_END_DECLS

#endif /* !GUTACHTER_SUITE_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
