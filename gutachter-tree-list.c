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

/* GtkTreeIter format:
 * - stamp:      the stamp of the model (initially, a random number; increased
 *               by one for every move, delete and insert before existing rows)
 * - user_data:  the pointer to this model
 * - user_data2: the pointer to the child model
 * - user_data3: GINT_TO_POINTER() of the actual index
 */

struct _GutachterTreeListPrivate
{
  GtkTreeModel* model;
  GQueue      * references;
  gint32        stamp;
};

#define PRIV(i) (((GutachterTreeList*)(i))->_private)

enum
{
  PROP_0,
  PROP_MODEL
};

static void   implement_gtk_tree_model (GtkTreeModelIface* iface);
static GList* find_glist_for_tree_path (GQueue           * queue,
                                        GtkTreePath      * path);

G_DEFINE_TYPE_WITH_CODE (GutachterTreeList, gutachter_tree_list, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, implement_gtk_tree_model));

static void
gutachter_tree_list_init (GutachterTreeList* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GUTACHTER_TYPE_TREE_LIST, GutachterTreeListPrivate);
  PRIV (self)->references = g_queue_new ();
  PRIV (self)->stamp      = g_random_int ();
}

static void
finalize (GObject* object)
{
  g_object_unref (PRIV (object)->model);

  G_OBJECT_CLASS (gutachter_tree_list_parent_class)->finalize (object);
}

static int
compare_references (gconstpointer a,
                    gconstpointer b,
                    gpointer      user_data G_GNUC_UNUSED)
{
  GtkTreePath* path_a = gtk_tree_row_reference_get_path ((GtkTreeRowReference*)a);
  GtkTreePath* path_b = gtk_tree_row_reference_get_path ((GtkTreeRowReference*)b);
  int          result = gtk_tree_path_compare (path_a, path_b);

  gtk_tree_path_free (path_a);
  gtk_tree_path_free (path_b);

  return result;
}

static inline gboolean
validate_iter (GutachterTreeList* self,
               GtkTreeIter      * iter)
{
  return iter->stamp == PRIV (self)->stamp &&
         iter->user_data == self &&
         iter->user_data2 == PRIV (self)->model &&
         GPOINTER_TO_INT (iter->user_data3) >= 0 &&
         GPOINTER_TO_UINT (iter->user_data3) < g_queue_get_length (PRIV (self)->references);
}

static inline gboolean
initialize_iter (GutachterTreeList* self,
                 GtkTreeIter      * iter,
                 gint               flat_index)
{
  iter->stamp = PRIV (self)->stamp;
  iter->user_data = self;
  iter->user_data2 = PRIV (self)->model;
  iter->user_data3 = GINT_TO_POINTER (flat_index);

  return validate_iter (self, iter);
}

static void
row_changed_cb (GtkTreeModel* model     G_GNUC_UNUSED,
                GtkTreePath * path,
                GtkTreeIter * iter      G_GNUC_UNUSED,
                gpointer      user_data)
{
  GutachterTreeList* self = user_data;
  GtkTreePath      * our_path;
  GtkTreeIter        our_iter;
  GList            * link;

  /* FIXME: use gutachter_tree_list_path_from_child() */
  link = find_glist_for_tree_path (PRIV (self)->references, path);
  g_assert (link);
  our_path = gtk_tree_path_new_from_indices (g_queue_link_index (PRIV (self)->references, link), -1);
  g_assert (gtk_tree_model_get_iter (user_data, &our_iter, our_path));
  gtk_tree_model_row_changed (user_data, our_path, &our_iter);
  gtk_tree_path_free (our_path);
}

static void
row_inserted_cb (GtkTreeModel* model,
                 GtkTreePath * path,
                 GtkTreeIter * iter      G_GNUC_UNUSED,
                 gpointer      user_data)
{
  GtkTreeRowReference* reference = gtk_tree_row_reference_new (model, path);
  GutachterTreeList  * self = user_data;
  GtkTreePath        * our_path;
  GtkTreeIter          our_iter;

  g_queue_insert_sorted (PRIV (self)->references,
                         reference,
                         compare_references,
                         NULL);

  our_path = gtk_tree_path_new_from_indices (g_queue_index (PRIV (self)->references, reference),
                                             -1);
  g_assert (initialize_iter (self, &our_iter, gtk_tree_path_get_indices (our_path)[0]));
  gtk_tree_model_row_inserted (GTK_TREE_MODEL (self), our_path, &our_iter);
  gtk_tree_path_free (our_path);
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

      g_signal_connect (PRIV (object)->model, "row-changed",
                        G_CALLBACK (row_changed_cb), object);
      g_signal_connect (PRIV (object)->model, "row-inserted",
                        G_CALLBACK (row_inserted_cb), object);
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

static GtkTreeModelFlags
get_flags (GtkTreeModel* model G_GNUC_UNUSED)
{
  return GTK_TREE_MODEL_LIST_ONLY;
}

static gint
get_n_columns (GtkTreeModel* model)
{
  return gtk_tree_model_get_n_columns (PRIV (model)->model);
}

static GType
get_column_type (GtkTreeModel* model, gint index)
{
  return gtk_tree_model_get_column_type (PRIV (model)->model, index);
}

static int
compare_reference_with_path (gconstpointer a,
                             gconstpointer b)
{
  GtkTreePath* path = gtk_tree_row_reference_get_path ((GtkTreeRowReference*) a);
  int          result = gtk_tree_path_compare (path, b);

  gtk_tree_path_free (path);

  return result;
}

static GList*
find_glist_for_tree_path (GQueue     * queue,
                          GtkTreePath* path)
{
  return g_queue_find_custom (queue, path, compare_reference_with_path);
}

static gboolean
get_iter (GtkTreeModel* model,
          GtkTreeIter * iter,
          GtkTreePath * path)
{
  if (gtk_tree_path_get_depth (path) != 1)
    {
      g_debug ("tree path too deep");
      return FALSE;
    }

  return initialize_iter (GUTACHTER_TREE_LIST (model), iter, gtk_tree_path_get_indices (path)[0]);
}

static GtkTreePath*
get_path (GtkTreeModel* model,
          GtkTreeIter * iter)
{
  g_return_val_if_fail (validate_iter (GUTACHTER_TREE_LIST (model), iter), NULL);

  return gtk_tree_path_new_from_indices (GPOINTER_TO_INT (iter->user_data3), -1);
}

static void
get_value (GtkTreeModel* model,
           GtkTreeIter * iter,
           gint          column,
           GValue      * value)
{
  GtkTreeIter  child_iter;

  g_return_if_fail (gutachter_tree_list_iter_to_child (GUTACHTER_TREE_LIST (model), &child_iter, iter));

  gtk_tree_model_get_value (PRIV (model)->model,
                            &child_iter,
                            column,
                            value);
}

static gboolean
iter_children (GtkTreeModel* model,
               GtkTreeIter * iter,
               GtkTreeIter * parent)
{
  return gtk_tree_model_iter_nth_child (model, iter, parent, 0);
}

static gboolean
iter_has_child (GtkTreeModel* model G_GNUC_UNUSED,
                GtkTreeIter * iter)
{
  g_assert (iter); /* gtk_tree_model_iter_has_child() should catch NULL already */
  return FALSE;
}

static gint
iter_n_children (GtkTreeModel* model,
                 GtkTreeIter * iter)
{
  if (iter)
    {
      return FALSE;
    }

  return g_queue_get_length (PRIV (model)->references);
}

static gboolean
iter_next (GtkTreeModel* model,
           GtkTreeIter * iter)
{
  if (!validate_iter (GUTACHTER_TREE_LIST (model), iter))
    {
      return FALSE;
    }

  iter->user_data3 = GINT_TO_POINTER (GPOINTER_TO_INT (iter->user_data3) + 1);

  return validate_iter (GUTACHTER_TREE_LIST (model), iter);
}

static gboolean
iter_nth_child (GtkTreeModel* model,
                GtkTreeIter * iter,
                GtkTreeIter * parent,
                gint          index)
{
  if (parent)
    {
      return FALSE;
    }

  return initialize_iter (GUTACHTER_TREE_LIST (model), iter, index);
}

static gboolean
iter_parent (GtkTreeModel* model  G_GNUC_UNUSED,
             GtkTreeIter * parent G_GNUC_UNUSED,
             GtkTreeIter * child  G_GNUC_UNUSED)
{
  return FALSE;
}

static void
implement_gtk_tree_model (GtkTreeModelIface* iface)
{
  iface->get_flags       = get_flags;
  iface->get_n_columns   = get_n_columns;
  iface->get_column_type = get_column_type;
  iface->get_iter        = get_iter;
  iface->get_path        = get_path;
  iface->get_value       = get_value;
  iface->iter_children   = iter_children;
  iface->iter_has_child  = iter_has_child;
  iface->iter_n_children = iter_n_children;
  iface->iter_next       = iter_next;
  iface->iter_nth_child  = iter_nth_child;
  iface->iter_parent     = iter_parent;
}

GtkTreeModel*
gutachter_tree_list_get_model (GutachterTreeList* self)
{
  g_return_val_if_fail (GUTACHTER_IS_TREE_LIST (self), NULL);

  return PRIV (self)->model;
}

gboolean
gutachter_tree_list_iter_from_child (GutachterTreeList* self,
                                     GtkTreeIter      * iter,
                                     GtkTreeIter      * child_iter)
{
  GtkTreePath* path = gtk_tree_model_get_path (PRIV (self)->model, child_iter);
  gboolean     result;
  GList      * link;

  if (!path)
    {
      return FALSE;
    }

  link = find_glist_for_tree_path (PRIV (self)->references, path);
  if (!link)
    {
      gtk_tree_path_free (path);
      return FALSE;
    }

  gtk_tree_path_free (path);
  path = gtk_tree_path_new_from_indices (g_queue_link_index (PRIV (self)->references, link), -1);
  result = gtk_tree_model_get_iter (GTK_TREE_MODEL (self), iter, path);
  gtk_tree_path_free (path);

  return result;
}

gboolean
gutachter_tree_list_iter_to_child (GutachterTreeList* self,
                                   GtkTreeIter      * child_iter,
                                   GtkTreeIter      * iter)
{
  GtkTreeRowReference* reference;
  GtkTreePath* path;
  gboolean     result;

  g_return_val_if_fail (GUTACHTER_IS_TREE_LIST (self), FALSE);
  if (!validate_iter (self, iter))
    {
      return FALSE;
    }

  reference = g_queue_peek_nth (PRIV (self)->references, GPOINTER_TO_INT (iter->user_data3));
  if (!reference)
    {
      return FALSE;
    }

  path = gtk_tree_row_reference_get_path (reference);
  result = gtk_tree_model_get_iter (PRIV (self)->model, child_iter, path);
  gtk_tree_path_free (path);

  return result;
}

GtkTreeModel*
gutachter_tree_list_new (GtkTreeModel* model)
{
  return g_object_new (GUTACHTER_TYPE_TREE_LIST,
                       "model", model,
                       NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
