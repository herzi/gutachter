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

#include "gutachter-tree-list.h"

static void implement_gtk_tree_model (GtkTreeModelIface* iface);

G_DEFINE_TYPE_WITH_CODE (GutachterTreeList, gutachter_tree_list, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, implement_gtk_tree_model));

static void
gutachter_tree_list_init (GutachterTreeList* self G_GNUC_UNUSED)
{}

static void
gutachter_tree_list_class_init (GutachterTreeListClass* self_class G_GNUC_UNUSED)
{}

GtkTreeModelFlags
get_flags (GtkTreeModel* model G_GNUC_UNUSED)
{
  return GTK_TREE_MODEL_LIST_ONLY;
}

static void
implement_gtk_tree_model (GtkTreeModelIface* iface)
{
  iface->get_flags = get_flags;
}

GtkTreeModel*
gutachter_tree_list_new (GtkTreeModel* real_tree G_GNUC_UNUSED)
{
  return g_object_new (GUTACHTER_TYPE_TREE_LIST,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
