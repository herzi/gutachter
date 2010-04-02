/* This file is part of herzi's playground
 *
 * Copyright (C) 2010  Sven Herzberg
 *
 * This work is provided "as is"; redistribution and modification
 * in whole or in part, in any medium, physical or electronic is
 * permitted without restriction.
 *
 * This work is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * In no event shall the authors or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 */

#include <gutachter.h>

#include <string.h> /* memset() */

#define gutachter_assert_cmppath(p1, cmp, p2)    do { \
                                                     if (gtk_tree_path_compare (p1, p2) cmp 0) ; \
                                                     else { \
                                                         gchar* __s1 = gtk_tree_path_to_string (p1); \
                                                         gchar* __s2 = gtk_tree_path_to_string (p2); \
                                                         g_assertion_message_cmpstr (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                                                     #p1 " " #cmp " " #p2, __s1, #cmp, __s2); \
                                                         g_free (__s2); \
                                                         g_free (__s1); \
                                                     } \
                                                 } while (0)

static void
test_bar_init (void)
{
  GtkRequisition  req = {0, 0};
  GtkAllocation   alloc;
  GtkWidget* subject = gtk_test_create_widget (GUTACHTER_TYPE_BAR, NULL);
  GtkWidget* child = gtk_test_create_widget (GTK_TYPE_LABEL, "label", "TEST", NULL);

  gtk_widget_size_request (subject, &req);
  g_assert_cmpint (0, <, req.width);
  g_assert_cmpint (0, <, req.height);

  gtk_container_add (GTK_CONTAINER (subject), child);
  gtk_widget_size_request (subject, &req);

  alloc.x = 0;
  alloc.y = 0;
  alloc.width = req.width;
  alloc.height = req.height;
  gtk_widget_size_allocate (subject, &alloc);

  gtk_widget_get_allocation (child, &alloc);
  g_assert_cmpint (6, ==, alloc.x);
  g_assert_cmpint (6, ==, alloc.y);
  g_assert_cmpint (0, <, alloc.width);
  g_assert_cmpint (0, <, alloc.height);
}

static void
test_tree_list_init (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));

  g_assert (G_IS_OBJECT (subject));

  g_object_unref (subject);
  g_object_unref (store);
}

static void
populate_shopping_list (GtkTreeStore* store)
{
  GtkTreeIter  iter1;
  GtkTreeIter  iter2;

  gtk_tree_store_append (store, &iter1, NULL);
  gtk_tree_store_set (store, &iter1,
                      0, "Sliffy Supermarkt",
                      -1);

  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Milk",
                      -1);
  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Cheese",
                      -1);

  gtk_tree_store_append (store, &iter1, NULL);
  gtk_tree_store_set (store, &iter1,
                      0, "Sloff Shop",
                      -1);

  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Noodles",
                      -1);
  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Potatoes",
                      -1);
}

static void
test_tree_list_iter_from_child (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));
  GtkTreeIter   iter_item;
  GtkTreeIter   iter_mall;
  GtkTreeIter   iter;

  populate_shopping_list (store);

  g_assert (gtk_tree_model_iter_children (GTK_TREE_MODEL (store), &iter_mall, NULL));
  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter, &iter_mall));
  g_assert (gtk_tree_model_iter_children (GTK_TREE_MODEL (store), &iter_item, &iter_mall));
  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter, &iter_item));
  g_assert (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter_item));
  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter, &iter_item));
  g_assert (!gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter_item));

  g_assert (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter_mall));
  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter, &iter_mall));
  g_assert (gtk_tree_model_iter_children (GTK_TREE_MODEL (store), &iter_item, &iter_mall));
  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter, &iter_item));
  g_assert (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter_item));
  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter, &iter_item));
  g_assert (!gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter_item));

  g_assert (!gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter_mall));

  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_list_iter_to_child (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));
  GtkTreePath * path;
  GtkTreeIter   iter_item;
  GtkTreeIter   iter_mall;
  GtkTreeIter   iter;

  populate_shopping_list (store);

  path = gtk_tree_path_new_from_indices (0, -1);
  g_assert (gtk_tree_model_get_iter (subject, &iter, path));
  g_assert (gutachter_tree_list_iter_to_child (GUTACHTER_TREE_LIST (subject), &iter_mall, &iter));

  gtk_tree_path_next (path);
  g_assert (gtk_tree_model_get_iter (subject, &iter, path));
  g_assert (gutachter_tree_list_iter_to_child (GUTACHTER_TREE_LIST (subject), &iter_item, &iter));

  gtk_tree_path_next (path);
  if (g_test_verbose ())
    {
      gchar* string = gtk_tree_path_to_string (path);
      g_message ("trying path %s", string);
      g_free (string);
    }
  g_assert (gtk_tree_model_get_iter (subject, &iter, path));
  g_assert (gutachter_tree_list_iter_to_child (GUTACHTER_TREE_LIST (subject), &iter_item, &iter));

  gtk_tree_path_next (path);
  g_assert (gtk_tree_model_get_iter (subject, &iter, path));
  g_assert (gutachter_tree_list_iter_to_child (GUTACHTER_TREE_LIST (subject), &iter_mall, &iter));

  gtk_tree_path_next (path);
  g_assert (gtk_tree_model_get_iter (subject, &iter, path));
  g_assert (gutachter_tree_list_iter_to_child (GUTACHTER_TREE_LIST (subject), &iter_item, &iter));

  gtk_tree_path_next (path);
  g_assert (gtk_tree_model_get_iter (subject, &iter, path));
  g_assert (gutachter_tree_list_iter_to_child (GUTACHTER_TREE_LIST (subject), &iter_item, &iter));

  gtk_tree_path_free (path);
  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_list_model_type (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));

  g_assert (GTK_IS_TREE_MODEL (subject));

  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_list_model_get_flags (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));

  g_assert_cmpint (GTK_TREE_MODEL_LIST_ONLY, ==, gtk_tree_model_get_flags (subject));

  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_list_model_get_n_columns (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));

  g_assert_cmpint (gtk_tree_model_get_n_columns (GTK_TREE_MODEL (store)), ==,
                   gtk_tree_model_get_n_columns (subject));

  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_list_model_get_column_type (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));
  gint column;

  for (column = 0; column < gtk_tree_model_get_n_columns (subject); column++)
    {
      if (g_test_verbose ())
        {
          g_test_message ("testing column index %d", column);
        }

      g_assert_cmpint (gtk_tree_model_get_column_type (GTK_TREE_MODEL (store), column), ==,
                       gtk_tree_model_get_column_type (subject, column));
    }

  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_list_model_get_iter (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));
  GtkTreeIter   iter1;
  GtkTreeIter   iter2;
  GtkTreeIter   iter3;
  GtkTreePath * path = gtk_tree_path_new ();

  gtk_tree_path_append_index (path, 0);
  g_assert_cmpint (FALSE, ==, gtk_tree_model_get_iter (subject, &iter3, path));
  gtk_tree_store_append (store, &iter1, NULL);
  gtk_tree_store_set (store, &iter1,
                      0, "Sliffy Supermarkt",
                      -1);
  g_assert_cmpint (TRUE, ==, gtk_tree_model_get_iter (subject, &iter3, path));

  gtk_tree_path_next (path);
  g_assert_cmpint (FALSE, ==, gtk_tree_model_get_iter (subject, &iter3, path));
  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Milk",
                      -1);
  g_assert_cmpint (TRUE, ==, gtk_tree_model_get_iter (subject, &iter3, path));
  gtk_tree_path_next (path);
  g_assert_cmpint (FALSE, ==, gtk_tree_model_get_iter (subject, &iter3, path));
  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Cheese",
                      -1);
  g_assert_cmpint (TRUE, ==, gtk_tree_model_get_iter (subject, &iter3, path));

  gtk_tree_path_next (path);
  g_assert_cmpint (FALSE, ==, gtk_tree_model_get_iter (subject, &iter3, path));
  gtk_tree_store_append (store, &iter1, NULL);
  gtk_tree_store_set (store, &iter1,
                      0, "Sloff Shop",
                      -1);
  g_assert_cmpint (TRUE, ==, gtk_tree_model_get_iter (subject, &iter3, path));

  gtk_tree_path_next (path);
  g_assert_cmpint (FALSE, ==, gtk_tree_model_get_iter (subject, &iter3, path));
  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Noodles",
                      -1);
  g_assert_cmpint (TRUE, ==, gtk_tree_model_get_iter (subject, &iter3, path));
  gtk_tree_path_next (path);
  g_assert_cmpint (FALSE, ==, gtk_tree_model_get_iter (subject, &iter3, path));
  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Potatoes",
                      -1);
  g_assert_cmpint (TRUE, ==, gtk_tree_model_get_iter (subject, &iter3, path));

  gtk_tree_path_next (path);
  g_assert_cmpint (FALSE, ==, gtk_tree_model_get_iter (subject, &iter3, path));

  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_list_model_get_path (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));
  GtkTreePath * reference;
  GtkTreePath * selector;
  GtkTreeIter   iter1;
  GtkTreeIter   iter2;
  GtkTreeIter   iter3;

  selector = gtk_tree_path_new ();
  gtk_tree_store_append (store, &iter1, NULL);
  gtk_tree_store_set (store, &iter1,
                      0, "Sliffy Supermarkt",
                      -1);

  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter3, &iter1));
  gtk_tree_path_append_index (selector, 0);
  reference = gtk_tree_model_get_path (subject, &iter3);
  gutachter_assert_cmppath (selector, ==, reference);
  gtk_tree_path_free (reference);

  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Milk",
                      -1);

  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter3, &iter2));
  gtk_tree_path_next (selector);
  reference = gtk_tree_model_get_path (subject, &iter3);
  gutachter_assert_cmppath (selector, ==, reference);
  gtk_tree_path_free (reference);

  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Cheese",
                      -1);

  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter3, &iter2));
  gtk_tree_path_next (selector);
  reference = gtk_tree_model_get_path (subject, &iter3);
  gutachter_assert_cmppath (selector, ==, reference);
  gtk_tree_path_free (reference);

  gtk_tree_store_append (store, &iter1, NULL);
  gtk_tree_store_set (store, &iter1,
                      0, "Sloff Shop",
                      -1);

  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter3, &iter1));
  gtk_tree_path_next (selector);
  reference = gtk_tree_model_get_path (subject, &iter3);
  gutachter_assert_cmppath (selector, ==, reference);
  gtk_tree_path_free (reference);

  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Noodles",
                      -1);

  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter3, &iter2));
  gtk_tree_path_next (selector);
  reference = gtk_tree_model_get_path (subject, &iter3);
  gutachter_assert_cmppath (selector, ==, reference);
  gtk_tree_path_free (reference);

  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      0, "Potatoes",
                      -1);

  g_assert (gutachter_tree_list_iter_from_child (GUTACHTER_TREE_LIST (subject), &iter3, &iter2));
  gtk_tree_path_next (selector);
  reference = gtk_tree_model_get_path (subject, &iter3);
  gutachter_assert_cmppath (selector, ==, reference);
  gtk_tree_path_free (reference);

  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_list_model_iter_nth_child (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));
  GtkTreeIter   iter;
  GtkTreeIter   child_iter;
  gint          index = 0;

  g_assert (!gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));

  populate_shopping_list (store);

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_nth_child (subject, &child_iter, &iter, 0));
  index++;

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_nth_child (subject, &child_iter, &iter, 0));
  index++;

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_nth_child (subject, &child_iter, &iter, 0));
  index++;

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_nth_child (subject, &child_iter, &iter, 0));
  index++;

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_nth_child (subject, &child_iter, &iter, 0));
  index++;

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_nth_child (subject, &child_iter, &iter, 0));
  index++;

  g_assert (!gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));

  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_list_model_iter_children (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));
  GtkTreeIter   iter;
  GtkTreeIter   child_iter;
  gint          index = 0;

  g_assert (!gtk_tree_model_iter_children (subject, &iter, NULL));

  populate_shopping_list (store);

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_children (subject, &child_iter, &iter));
  index++;

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_children (subject, &child_iter, &iter));
  index++;

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_children (subject, &child_iter, &iter));
  index++;

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_children (subject, &child_iter, &iter));
  index++;

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_children (subject, &child_iter, &iter));
  index++;

  g_assert (gtk_tree_model_iter_nth_child (subject, &iter, NULL, index));
  g_assert (!gtk_tree_model_iter_children (subject, &child_iter, &iter));

  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_list_model_iter_next (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));
  GtkTreeIter   iter;

  populate_shopping_list (store);

  g_assert (gtk_tree_model_iter_children (subject, &iter, NULL));
  g_assert (gtk_tree_model_iter_next (subject, &iter));
  g_assert (gtk_tree_model_iter_next (subject, &iter));
  g_assert (gtk_tree_model_iter_next (subject, &iter));
  g_assert (gtk_tree_model_iter_next (subject, &iter));
  g_assert (gtk_tree_model_iter_next (subject, &iter));
  g_assert (!gtk_tree_model_iter_next (subject, &iter));

  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_list_model_get_value (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));
  GtkTreeIter   iter;
  gboolean      valid_iter;
  GValue        parent_value;
  GValue        child_value;

  memset (&parent_value, '\0', sizeof (parent_value));
  memset (&child_value, '\0', sizeof (child_value));

  populate_shopping_list (store);
  for (valid_iter = gtk_tree_model_iter_children (subject, &iter, NULL);
       valid_iter;
       valid_iter = gtk_tree_model_iter_next (subject, &iter))
    {
      GtkTreeIter  child_iter;

      g_assert (gutachter_tree_list_iter_to_child (GUTACHTER_TREE_LIST (subject), &child_iter, &iter));

      gtk_tree_model_get_value (GTK_TREE_MODEL (store), &child_iter, 0, &child_value);
      gtk_tree_model_get_value (subject, &iter, 0, &parent_value);

      g_assert_cmpint (G_VALUE_TYPE (&parent_value), ==, G_TYPE_STRING);
      g_assert_cmpint (G_VALUE_TYPE (&child_value), ==, G_TYPE_STRING);

      g_assert_cmpstr (g_value_get_string (&parent_value), ==, g_value_get_string (&child_value));

      g_value_unset (&parent_value);
      g_value_unset (&child_value);
    }

  g_object_unref (subject);
  g_object_unref (store);
}

static void
test_tree_model_iter_has_child (void)
{
  GtkTreeStore* store = gtk_tree_store_new (1, G_TYPE_STRING);
  GtkTreeModel* subject = gutachter_tree_list_new (GTK_TREE_MODEL (store));
  GtkTreeIter   iter;

  populate_shopping_list (store);
  g_assert (gtk_tree_model_iter_children (subject, &iter, NULL));
  g_assert (!gtk_tree_model_iter_has_child (subject, &iter));
  g_assert (gtk_tree_model_iter_next (subject, &iter));
  g_assert (!gtk_tree_model_iter_has_child (subject, &iter));
  g_assert (gtk_tree_model_iter_next (subject, &iter));
  g_assert (!gtk_tree_model_iter_has_child (subject, &iter));
  g_assert (gtk_tree_model_iter_next (subject, &iter));
  g_assert (!gtk_tree_model_iter_has_child (subject, &iter));
  g_assert (gtk_tree_model_iter_next (subject, &iter));
  g_assert (!gtk_tree_model_iter_has_child (subject, &iter));
  g_assert (gtk_tree_model_iter_next (subject, &iter));
  g_assert (!gtk_tree_model_iter_has_child (subject, &iter));
  g_assert (!gtk_tree_model_iter_next (subject, &iter));

  g_object_unref (subject);
  g_object_unref (store);
}

int
main (int   argc,
      char**argv)
{
  gtk_test_init (&argc, &argv, NULL);

  g_test_add_func ("/com/github/herzi/gutachter/GutachterBar/init", test_bar_init);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/init", test_tree_list_init);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/GtkTreeModel/type", test_tree_list_model_type);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/GtkTreeModel/API/get-flags", test_tree_list_model_get_flags);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/GtkTreeModel/API/get-n-columns", test_tree_list_model_get_n_columns);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/GtkTreeModel/API/get-column-type", test_tree_list_model_get_column_type);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/GtkTreeModel/API/get-iter", test_tree_list_model_get_iter);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/iter-from-child", test_tree_list_iter_from_child);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/GtkTreeModel/API/get-path", test_tree_list_model_get_path);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/GtkTreeModel/API/iter-nth-child", test_tree_list_model_iter_nth_child);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/GtkTreeModel/API/iter-children", test_tree_list_model_iter_children);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/GtkTreeModel/API/iter-next", test_tree_list_model_iter_next);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/iter-to-child", test_tree_list_iter_to_child);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/GtkTreeModel/API/get-value", test_tree_list_model_get_value);
  g_test_add_func ("/com/github/herzi/gutachter/GutachterTreeList/GtkTreeModel/API/iter-has-child", test_tree_model_iter_has_child);
  /* API/iter-n-children */
  /* API/iter-parent */
  /* API/ref-node */
  /* API/unref-node */
  /* signals/row-changed */
  /* signals/row-inserted */
  /* signals/row-has-child-toggled */
  /* signals/row-deleted */
  /* signals/rows-reordered */
  /* "/com/github/herzi/gutachter/GutachterTreeList/path-from-child" */
  /* "/com/github/herzi/gutachter/GutachterTreeList/path-to-child" */

  /* FIXME: test proper handling of this: "dummy/a", "/dummy/b" */
  /* FIXME: test proper handling of this: "/com/github/herzi/gutachter/a", "com.github.herzi.gutachter.a" */
  /* FIXME: test proper handling of this: "/dummy", "/dummy" */
  /* FIXME: test testcase loading, unloading, refreshing and executing */
  /* FIXME: add code coverage reports */

  return g_test_run ();
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
