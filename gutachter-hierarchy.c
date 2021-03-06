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

struct _GutachterHierarchyPrivate
{
  GHashTable* path_to_reference;
};

enum
{
  COL_FULL_PATH = GUTACHTER_HIERARCHY_N_COLUMNS,
  COL_MESSAGE,
  COL_IS_TESTCASE,
  N_COLUMNS
};

#define PRIV(i) (((GutachterHierarchy*)(i))->_private)

G_DEFINE_TYPE (GutachterHierarchy, gutachter_hierarchy, GTK_TYPE_TREE_STORE);

static void
gutachter_hierarchy_init (GutachterHierarchy* self)
{
  GType types[N_COLUMNS] = {
          G_TYPE_STRING,
          G_TYPE_BOOLEAN,
          G_TYPE_BOOLEAN,
          G_TYPE_STRING,
          G_TYPE_GSTRING,
          G_TYPE_BOOLEAN
  };

  gtk_tree_store_set_column_types (GTK_TREE_STORE (self), N_COLUMNS, types);

  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, GUTACHTER_TYPE_HIERARCHY, GutachterHierarchyPrivate);
  PRIV (self)->path_to_reference = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GFreeFunc)gtk_tree_row_reference_free);
}

static void
finalize (GObject* object)
{
  g_hash_table_destroy (PRIV (object)->path_to_reference);

  G_OBJECT_CLASS (gutachter_hierarchy_parent_class)->finalize (object);
}
static void
gutachter_hierarchy_class_init (GutachterHierarchyClass* self_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS (self_class);

  object_class->finalize = finalize;

  g_type_class_add_private (self_class, sizeof (GutachterHierarchyPrivate));
}

/**
 * gutachter_hierarchy_append_message:
 * @self: a #GutachterHierarchy
 * @iter: a valid #GtkTreeIter for @self
 * @message: the message to be appended
 *
 * Append a message to the current output for a test. If @message is %NULL,
 * the empty string ("") will be appended.
 */
void
gutachter_hierarchy_append_message (GutachterHierarchy* self,
                                    GtkTreeIter       * iter,
                                    gchar const       * message)
{
  GString* stored = NULL;

  g_return_if_fail (GUTACHTER_IS_HIERARCHY (self));
  g_return_if_fail (gtk_tree_store_iter_is_valid (GTK_TREE_STORE (self), iter));

  gtk_tree_model_get (GTK_TREE_MODEL (self), iter,
                      COL_MESSAGE, &stored,
                      -1);

  if (!message)
    {
      message = "";
    }

  if (!stored)
    {
      stored = g_string_new (message);
    }
  else
    {
      g_string_append (stored, "\n");
      g_string_append (stored, message);
    }

  gtk_tree_store_set (GTK_TREE_STORE (self), iter,
                      COL_MESSAGE, stored,
                      -1);

  g_string_free (stored, TRUE);
}

/**
 * gutachter_hierarchy_clear:
 * @self: a #GutachterHierarchy
 *
 * Remove all children of the hierarchy.
 */
void
gutachter_hierarchy_clear (GutachterHierarchy* self)
{
  g_return_if_fail (GUTACHTER_IS_HIERARCHY (self));

  g_hash_table_remove_all (PRIV (self)->path_to_reference);
  gtk_tree_store_clear (GTK_TREE_STORE (self));
}

gchar*
gutachter_hierarchy_get_full_path (GutachterHierarchy* self,
                                   GtkTreeIter       * iter)
{
  gchar* result = NULL;

  g_return_val_if_fail (GUTACHTER_IS_HIERARCHY (self), NULL);
  g_return_val_if_fail (gtk_tree_store_iter_is_valid (GTK_TREE_STORE (self), iter), NULL);

  gtk_tree_model_get (GTK_TREE_MODEL (self), iter,
                      COL_FULL_PATH, &result,
                      -1);

  return result;
}

static void
get_iter_internal (GutachterHierarchy* self,
                   GtkTreeIter       * iter,
                   gchar const       * path,
                   gboolean            is_testcase)
{
  GtkTreeRowReference* reference;
  GtkTreeStore       * store = GTK_TREE_STORE (self);
  GtkTreePath        * tree_path;
  gchar              * last_slash;

  if (gutachter_hierarchy_lookup_iter (self, iter, path))
    {
      /* FIXME: test adding /sliff/sloff and the /sliff */
      return;
    }

  last_slash = g_strrstr (path, "/");
  if (!last_slash || last_slash == path || *(last_slash + 1) == '\0')
    {
      if (last_slash == path)
        {
          /* drop initial slashes */
          last_slash++;
        }
      gtk_tree_store_append (store, iter, NULL);
    }
  else
    {
      GtkTreeIter  parent;
      gchar* parent_path = g_strdup (path);
      gchar* last_slash_in_parent = parent_path + (last_slash - path);

      *last_slash_in_parent = '\0';
      get_iter_internal (self, &parent, parent_path, FALSE);
      *last_slash_in_parent = '/';

      last_slash++;

      gtk_tree_store_append (store, iter, &parent);
      g_free (parent_path);
    }

  gtk_tree_store_set (store, iter,
                      GUTACHTER_HIERARCHY_COLUMN_PASSED, FALSE,
                      GUTACHTER_HIERARCHY_COLUMN_UNSURE, TRUE,
                      GUTACHTER_HIERARCHY_COLUMN_NAME, last_slash,
                      COL_FULL_PATH, path,
                      COL_IS_TESTCASE, is_testcase,
                      -1);

  tree_path = gtk_tree_model_get_path (GTK_TREE_MODEL (store), iter);
  reference = gtk_tree_row_reference_new (GTK_TREE_MODEL (store), tree_path);
  g_hash_table_insert (PRIV (store)->path_to_reference, g_strdup (path), reference);
  gtk_tree_path_free (tree_path);
}

void
gutachter_hierarchy_get_iter (GutachterHierarchy* self,
                              GtkTreeIter       * iter,
                              gchar const       * path)
{
  get_iter_internal (self, iter, path, TRUE);
}

gchar*
gutachter_hierarchy_get_message (GutachterHierarchy* self,
                                 GtkTreeIter       * iter)
{
  GString* stored = NULL;
  gchar  * result = NULL;

  g_return_val_if_fail (GUTACHTER_IS_HIERARCHY (self), NULL);
  g_return_val_if_fail (gtk_tree_store_iter_is_valid (GTK_TREE_STORE (self), iter), NULL);

  gtk_tree_model_get (GTK_TREE_MODEL (self), iter,
                      COL_MESSAGE, &stored,
                      -1);

  if (stored)
    {
      result = stored->str;
      g_string_free (stored, FALSE);
    }

  return result;
}

gboolean
gutachter_hierarchy_is_testcase (GutachterHierarchy* self,
                                 GtkTreeIter       * iter)
{
  gboolean result = FALSE;

  g_return_val_if_fail (GUTACHTER_IS_HIERARCHY (self), FALSE);
  g_return_val_if_fail (gtk_tree_store_iter_is_valid (GTK_TREE_STORE (self), iter), FALSE);

  gtk_tree_model_get (GTK_TREE_MODEL (self), iter,
                      COL_IS_TESTCASE, &result,
                      -1);

  return result;
}

gboolean
gutachter_hierarchy_lookup_iter (GutachterHierarchy* self,
                                 GtkTreeIter       * iter,
                                 gchar const       * path)
{
  GtkTreeRowReference* reference = g_hash_table_lookup (PRIV (self)->path_to_reference, path);
  if (reference)
    {
      GtkTreeModel* model = GTK_TREE_MODEL (self);
      GtkTreePath * tree_path = gtk_tree_row_reference_get_path (reference);

      g_assert (gtk_tree_model_get_iter (model, iter, tree_path));
      gtk_tree_path_free (tree_path);
      return TRUE;
    }

  return FALSE;
}

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
                      COL_MESSAGE, NULL,
                      -1);

  return FALSE;
}

/**
 * gutachter_hierarchy_reset:
 * @self: a #GutachterHierarchy
 *
 * Reset the test results of this hierarchy.
 */
void
gutachter_hierarchy_reset (GutachterHierarchy* self)
{
  g_return_if_fail (GUTACHTER_IS_HIERARCHY (self));

  gtk_tree_model_foreach (GTK_TREE_MODEL (self), set_unsure, NULL);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
