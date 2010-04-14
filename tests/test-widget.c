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
test_layout (void)
{
  GtkWidget* widget = gtk_test_create_widget (GUTACHTER_TYPE_WIDGET, NULL);

  g_assert (GTK_IS_VBOX (widget));
  g_assert (GTK_IS_NOTEBOOK (gutachter_lookup_child (widget, "urn:gtk:GtkWidget[2]")));
  g_assert (GTK_IS_PANED (gutachter_lookup_child (widget, "urn:gtk:GtkWidget[2]:GtkWidget[0]")));
}

void
add_tests_for_widget (void)
{
  g_test_add_func (NAMESPACE "GutachterWidget/layout", test_layout);
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
