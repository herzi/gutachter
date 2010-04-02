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

#ifndef GUTACHTER_TREE_LIST_H
#define GUTACHTER_TREE_LIST_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GutachterTreeList        GutachterTreeList;
typedef struct _GutachterTreeListClass   GutachterTreeListClass;
typedef struct _GutachterTreeListPrivate GutachterTreeListPrivate;

#define GUTACHTER_TYPE_TREE_LIST         (gutachter_tree_list_get_type ())
#define GUTACHTER_TREE_LIST(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), GUTACHTER_TYPE_TREE_LIST, GutachterTreeList))
#define GUTACHTER_TREE_LIST_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), GUTACHTER_TYPE_TREE_LIST, GutachterTreeListClass))
#define GUTACHTER_IS_TREE_LIST(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), GUTACHTER_TYPE_TREE_LIST))
#define GUTACHTER_IS_TREE_LIST_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), GUTACHTER_TYPE_TREE_LIST))
#define GUTACHTER_TREE_LIST_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), GUTACHTER_TYPE_TREE_LIST, GutachterTreeList))

GType         gutachter_tree_list_get_type        (void);
GtkTreeModel* gutachter_tree_list_new             (GtkTreeModel* real_tree);
gboolean      gutachter_tree_list_iter_from_child (GutachterTreeList* self,
                                                   GtkTreeIter      * iter,
                                                   GtkTreeIter      * child_iter);
gboolean      gutachter_tree_list_iter_to_child   (GutachterTreeList* self,
                                                   GtkTreeIter      * child_iter,
                                                   GtkTreeIter      * iter);

struct _GutachterTreeList
{
  GObject                   base_class;
  GutachterTreeListPrivate* _private;
};

struct _GutachterTreeListClass
{
  GObjectClass              base_class;
};

G_END_DECLS

#endif /* !GUTACHTER_TREE_LIST_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
