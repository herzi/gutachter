/* This file is part of herzi's playground
 *
 * AUTHORS
 *     Sven Herzberg
 *
 * Copyright (C) 2009  Sven Herzberg
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

#ifndef TANGO_H
#define TANGO_H

#include <gdk/gdk.h>

G_BEGIN_DECLS

/* color codes from the Tango Color palette:
 * http://tango.freedesktop.org/Tango_Icon_Theme_Guidelines */

typedef enum {
  TANGO_COLOR_BUTTER,
  TANGO_COLOR_ORANGE,
  TANGO_COLOR_CHOCOLATE,
  TANGO_COLOR_CHAMELEON,
  TANGO_COLOR_SKY_BLUE,
  TANGO_COLOR_PLUM,
  TANGO_COLOR_SCARLET_RED,
  TANGO_COLOR_ALUMINIUM_1,
  TANGO_COLOR_ALUMINIUM_2,
  TANGO_N_COLORS
} TangoColor;

typedef enum {
  TANGO_SHADE_BRIGHT,
  TANGO_SHADE_NORMAL,
  TANGO_SHADE_DARK,
  TANGO_N_SHADES
} TangoShade;

void tango_cairo_set_source_color      (cairo_t   * cr,
                                        TangoColor  color,
                                        TangoShade  shade);

void tango_cairo_set_source_color_alpha (cairo_t   * cr,
                                         TangoColor  color,
                                         TangoShade  shade,
                                         double      alpha);

void tango_gdk_set_color (GdkColor  * target,
                          TangoColor  source,
                          TangoShade  shade);

G_END_DECLS

#endif /* !TANGO_H */

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
