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

int
main (int   argc,
      char**argv)
{
  gtk_test_init (&argc, &argv, NULL);

  g_test_add_func ("/com/github/herzi/gutachter/GutachterBar/init", test_bar_init);
  /* FIXME: test proper handling of this: "dummy/a", "/dummy/b" */
  /* FIXME: test proper handling of this: "/com/github/herzi/gutachter/a", "com.github.herzi.gutachter.a" */
  /* FIXME: test proper handling of this: "/dummy", "/dummy" */
  /* FIXME: test testcase loading, unloading, refreshing and executing */
  /* FIXME: add code coverage reports */

  return g_test_run ();
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
