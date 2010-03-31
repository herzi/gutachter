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

#ifndef GUTACHTER_BAR_H
#define GUTACHTER_BAR_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GutachterBar        GutachterBar;
typedef struct _GutachterBarClass   GutachterBarClass;
typedef struct _GutachterBarPrivate GutachterBarPrivate;

#define GUTACHTER_TYPE_BAR         (gutachter_bar_get_type ())
#define GUTACHTER_BAR(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), GUTACHTER_TYPE_BAR, GutachterBar))
#define GUTACHTER_BAR_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), GUTACHTER_TYPE_BAR, GutachterBarClass))
#define GUTACHTER_IS_BAR(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), GUTACHTER_TYPE_BAR))
#define GUTACHTER_IS_BAR_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), GUTACHTER_TYPE_BAR))
#define GUTACHTER_BAR_GET_CLASS(i) (G_TYPE_INSTANCE_GET_CLASS ((i), GUTACHTER_TYPE_BAR, GutachterBarClass))

GType      gutachter_bar_get_type (void);
GtkWidget* gutachter_bar_new      (void);

struct _GutachterBar
{
  GtkBin               base_instance;
  GutachterBarPrivate* _private;
};

struct _GutachterBarClass
{
  GtkBinClass          base_class;
};

G_END_DECLS

#endif /* !GUTACHTER_BAR_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
