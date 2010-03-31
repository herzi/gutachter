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

#include "gutachter-hierarchy.h"

G_DEFINE_TYPE (GutachterHierarchy, gutachter_hierarchy, GTK_TYPE_TREE_STORE);

static void
gutachter_hierarchy_init (GutachterHierarchy* self)
{
  GType types[GUTACHTER_HIERARCHY_N_COLUMNS] = {
          G_TYPE_STRING,
          G_TYPE_BOOLEAN,
          G_TYPE_BOOLEAN
  };
  gtk_tree_store_set_column_types (GTK_TREE_STORE (self), GUTACHTER_HIERARCHY_N_COLUMNS, types);
}

static void
gutachter_hierarchy_class_init (GutachterHierarchyClass* self_class G_GNUC_UNUSED)
{}

GutachterHierarchy*
gutachter_hierarchy_new (void)
{
  return g_object_new (GUTACHTER_TYPE_HIERARCHY,
                       NULL);
}

static gboolean
set_unsure (GtkTreeModel* model,
            GtkTreePath * path      G_GNUC_UNUSED,
            GtkTreeIter * iter,
            gpointer      user_data G_GNUC_UNUSED)
{
  gtk_tree_store_set (GTK_TREE_STORE (model), iter,
                      GUTACHTER_HIERARCHY_COLUMN_UNSURE, TRUE,
                      -1);
  return FALSE;
}

void
gutachter_hierarchy_set_unsure (GutachterHierarchy* self)
{
  g_return_if_fail (GUTACHTER_IS_HIERARCHY (self));

  gtk_tree_model_foreach (GTK_TREE_MODEL (self), set_unsure, NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
