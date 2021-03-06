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

#ifndef TEST_MAIN_H
#define TEST_MAIN_H

#include <gutachter.h>

G_BEGIN_DECLS

#define NAMESPACE "/com/github/herzi/gutachter/"

void add_tests_for_hierarchy (void);
void add_tests_for_lookup    (void);
void add_tests_for_xvfb      (void);
void add_tests_for_suite     (void);
void add_tests_for_widget    (void);

/* FIXME: make 64bit save and test this macro, too */
#define gutachter_assert_cmpptr(p1,cmp,p2)       g_assert_cmphex(GPOINTER_TO_INT (p1), cmp, GPOINTER_TO_INT (p2))
#define gutachter_assert_child(parent,path,type) if (!G_TYPE_CHECK_INSTANCE_TYPE (gutachter_lookup_child (parent,path), type)) {gchar* message = g_strdup_printf ("the widget at \"%s\" is a %s (expected was a %s)", path, G_OBJECT_TYPE_NAME (gutachter_lookup_child (parent,path)), g_type_name (type)); g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, message);}

G_END_DECLS

#endif /* !TEST_MAIN_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
