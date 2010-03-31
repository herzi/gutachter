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

#ifndef GUTACHTER_HIERARCHY_H
#define GUTACHTER_HIERARCHY_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GutachterHierarchy        GutachterHierarchy;
typedef struct _GutachterHierarchyClass   GutachterHierarchyClass;
typedef struct _GutachterHierarchyPrivate GutachterHierarchyPrivate;

#define GUTACHTER_TYPE_HIERARCHY         (gutachter_hierarchy_get_type ())
#define GUTACHTER_HIERARCHY(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), GUTACHTER_TYPE_HIERARCHY, GutachterHierarchy))
#define GUTACHTER_HIERARCHY_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), GUTACHTER_TYPE_HIERARCHY, GutachterHierarchyClass))
#define GUTACHTER_IS_HIERARCHY(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), GUTACHTER_TYPE_HIERARCHY))
#define GUTACHTER_IS_HIERARCHY_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), GUTACHTER_TYPE_HIERARCHY))
#define GUTACHTER_HIERARCHY_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), GUTACHTER_TYPE_HIERARCHY, GutachterHierarchyClass))

enum
{
  GUTACHTER_HIERARCHY_COLUMN_NAME,
  GUTACHTER_HIERARCHY_COLUMN_PASSED,
  GUTACHTER_HIERARCHY_COLUMN_UNSURE,
  GUTACHTER_HIERARCHY_N_COLUMNS
};

GType               gutachter_hierarchy_get_type    (void);
GutachterHierarchy* gutachter_hierarchy_new         (void);
void                gutachter_hierarchy_set_unsure  (GutachterHierarchy* self);
void                gutachter_hierarchy_set_message (GutachterHierarchy* self,
                                                     GtkTreeIter       * iter,
                                                     gchar const       * message);

struct _GutachterHierarchy
{
  GtkTreeStore               base_instance;
  GutachterHierarchyPrivate* _private;
};

struct _GutachterHierarchyClass
{
  GtkTreeStoreClass          base_class;
};

G_END_DECLS

#endif /* !GUTACHTER_HIERARCHY_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
