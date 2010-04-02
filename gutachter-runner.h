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

#ifndef GUTACHTER_RUNNER_H
#define GUTACHTER_RUNNER_H

#include <gio/gio.h>
#include <gutachter-suite.h>

G_BEGIN_DECLS

typedef struct _GutachterRunner      GutachterRunner;
typedef struct _GutachterRunnerIface GutachterRunnerIface;

#define GUTACHTER_TYPE_RUNNER         (gutachter_runner_get_type ())
#define GUTACHTER_RUNNER(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), GUTACHTER_TYPE_RUNNER, GutachterRunner))
#define GUTACHTER_IS_RUNNER(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), GUTACHTER_TYPE_RUNNER))
#define GUTACHTER_RUNNER_GET_IFACE(i) (G_TYPE_INSTANCE_GET_INTERFACE ((i), GUTACHTER_TYPE_RUNNER, GutachterRunnerIface))

GType           gutachter_runner_get_type  (void);
GFile*          gutachter_runner_get_file  (GutachterRunner* self);
GutachterSuite* gutachter_runner_get_suite (GutachterRunner* self);
void            gutachter_runner_set_file  (GutachterRunner* self,
                                            GFile          * file);

struct _GutachterRunnerIface
{
  GTypeInterface  base_interface;

  /* vtable */
  GFile*          (*get_file)  (GutachterRunner* self);
  GutachterSuite* (*get_suite) (GutachterRunner* self);
  void            (*set_file)  (GutachterRunner* self,
                                GFile          * file);
};

G_END_DECLS

#endif /* !GUTACHER_RUNNER_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
