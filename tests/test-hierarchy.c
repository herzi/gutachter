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

#include "test-main.h"

static void
test_append (void)
{
  GutachterHierarchy* cut = gutachter_hierarchy_new ();
  GtkTreeIter         iter;
  gchar             * msg;

  g_test_queue_unref (cut);

  gutachter_hierarchy_get_iter (cut, &iter, "/com/github/herzi/gutachter/dummy");
  gutachter_assert_cmpptr (NULL, ==, gutachter_hierarchy_get_message (cut, &iter));

  gutachter_hierarchy_append_message (cut, &iter, "test message A");
  msg = gutachter_hierarchy_get_message (cut, &iter);
  g_test_queue_free (msg);
  g_assert_cmpstr (msg, ==, "test message A");

  gutachter_hierarchy_append_message (cut, &iter, "test message B");
  msg = gutachter_hierarchy_get_message (cut, &iter);
  g_assert_cmpstr (msg, ==, "test message A\ntest message B");

  gutachter_hierarchy_reset (cut);
  g_assert_cmpstr (NULL, ==, gutachter_hierarchy_get_message (cut, &iter));
}

void
add_tests_for_hierarchy (void)
{
  g_test_add_func (NAMESPACE "Hierarchy/message", test_append);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
