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

struct _GutachterTreeListPrivate
{
  GtkTreeModel* model;
};

#define PRIV(i) (((GutachterTreeList*)(i))->_private)

enum
{
  PROP_0,
  PROP_MODEL
};

static void implement_gtk_tree_model (GtkTreeModelIface* iface);

G_DEFINE_TYPE_WITH_CODE (GutachterTreeList, gutachter_tree_list, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, implement_gtk_tree_model));

static void
gutachter_tree_list_init (GutachterTreeList* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GUTACHTER_TYPE_TREE_LIST, GutachterTreeListPrivate);
}

static void
finalize (GObject* object)
{
  g_object_unref (PRIV (object)->model);

  G_OBJECT_CLASS (gutachter_tree_list_parent_class)->finalize (object);
}

static void
set_property (GObject     * object,
              guint         prop_id,
              GValue const* value,
              GParamSpec  * pspec)
{
  switch (prop_id)
    {
    case PROP_MODEL:
      g_return_if_fail (!PRIV (object)->model);
      PRIV (object)->model = g_value_dup_object (value);
      /* construct-only => no notification */
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gutachter_tree_list_class_init (GutachterTreeListClass* self_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS (self_class);

  object_class->finalize     = finalize;
  object_class->set_property = set_property;

  g_object_class_install_property (object_class, PROP_MODEL,
                                   g_param_spec_object ("model", NULL, NULL,
                                                        GTK_TYPE_TREE_MODEL, G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

  g_type_class_add_private (self_class, sizeof (GutachterTreeList));
}

GtkTreeModelFlags
get_flags (GtkTreeModel* model G_GNUC_UNUSED)
{
  return GTK_TREE_MODEL_LIST_ONLY;
}

gint
get_n_columns (GtkTreeModel* model)
{
  return gtk_tree_model_get_n_columns (PRIV (model)->model);
}

static void
implement_gtk_tree_model (GtkTreeModelIface* iface)
{
  iface->get_flags     = get_flags;
  iface->get_n_columns = get_n_columns;
}

GtkTreeModel*
gutachter_tree_list_new (GtkTreeModel* model)
{
  return g_object_new (GUTACHTER_TYPE_TREE_LIST,
                       "model", model,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
