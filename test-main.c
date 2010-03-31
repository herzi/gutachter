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
  /* API/get-column-type */
  /* API/get-iter */
  /* API/get-path */
  /* API/get-value */
  /* API/iter-next */
  /* API/iter-children */
  /* API/iter-has-child */
  /* API/iter-n-children */
  /* API/iter-nth-child */
  /* API/iter-parent */
  /* API/ref-node */
  /* API/unref-node */
  /* signals/row-changed */
  /* signals/row-inserted */
  /* signals/row-has-child-toggled */
  /* signals/row-deleted */
  /* signals/rows-reordered */

  /* FIXME: test proper handling of this: "dummy/a", "/dummy/b" */
  /* FIXME: test proper handling of this: "/com/github/herzi/gutachter/a", "com.github.herzi.gutachter.a" */
  /* FIXME: test proper handling of this: "/dummy", "/dummy" */
  /* FIXME: test testcase loading, unloading, refreshing and executing */
  /* FIXME: add code coverage reports */

  return g_test_run ();
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
