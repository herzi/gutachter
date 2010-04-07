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

/* this file contains compatibility macros to make it possible to compile
 * code using the GTK+ 3.0 API with older GTK+ releases */

#ifndef COMPAT_GTK_H
#define COMPAT_GTK_H

#include <gtk/gtk.h>

#if !GTK_CHECK_VERSION(2,18,0)
#define gtk_widget_get_allocation(widget, allocation) *allocation = widget->allocation
#define gtk_widget_set_allocation(widget, allocation) widget->allocation = *allocation
#endif

#endif /* !COMPAT_GTK_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
