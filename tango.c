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

#include "tango.h"

static guint16 fixed[TANGO_N_COLORS][TANGO_N_SHADES][3] =
  {
    { /* Butter */
      {0xfcfc, 0xe9e9, 0x4f4f},
      /* FIXME */
      /* FIXME */
    },
    { /* Orange */
      /* FIXME */
    },
    { /* Chocolate */
      /* FIXME */
    },
    { /* Chameleon */
      {0x8a8a, 0xe2e2, 0x3434},
      {0x7373, 0xd2d2, 0x1616}
      /* FIXME */
    },
    { /* Sky Blue */
      {/* FIXME */},
      {0x3434, 0x6565, 0xA0A0},
      {/* FIXME */}
    },
    { /* Plum */
      /* FIXME */
    },
    { /* Scarlet Red */
      {0xefef, 0x2929, 0x2929},
      {0xcccc, 0x0000, 0x0000},
      /* FIXME */
    },
    { /* Aluminium (bright) */
      {0xeeee, 0xeeee, 0xecec},
      {/* FIXME */},
      {/* FIXME */}
    },
    { /* Aluminium (dark) */
      /* FIXME */
    }
  };

static double colors[TANGO_N_COLORS][TANGO_N_SHADES][3] = {{{0.0, 0.0, 0.0}}};

void
tango_cairo_set_source_color (cairo_t   * cr,
                              TangoColor  color,
                              TangoShade  shade)
{
  GdkColor  gdkcolor;

  tango_gdk_set_color (&gdkcolor, color, shade);
  gdk_cairo_set_source_color (cr, &gdkcolor);
}

void
tango_cairo_set_source_color_alpha (cairo_t   * cr,
                                    TangoColor  color,
                                    TangoShade  shade,
                                    double      alpha)
{
  g_return_if_fail (cr != NULL);
  g_return_if_fail (/* 0 <= color &&*/ color < TANGO_N_COLORS);
  g_return_if_fail (/* 0 <= shade &&*/ shade < TANGO_N_SHADES);

  if (G_UNLIKELY (colors[0][0][0] <= 0.0))
    {
      guint color;
      for (color = 0; color < TANGO_N_COLORS; color++)
        {
          guint shade;
          for (shade = 0; shade < TANGO_N_SHADES; shade++)
            {
              guint channel;

              for (channel = 0; channel < 3; channel++)
                {
                  colors[color][shade][channel] = fixed[color][shade][channel] * 1.0 / 0xFFFF;
                }
            }
        }
    }

  if (cairo_status (cr) != CAIRO_STATUS_SUCCESS)
    {
      /* don't waste time operating on broken contexts */
      return;
    }

  if (G_UNLIKELY (fixed[color][shade][0] == 0 && fixed[color][shade][1] == 0 && fixed[color][shade][2] == 0))
    {
      g_warning ("%s(%s): color %d and shade %d are not supported yet",
                 G_STRFUNC, G_STRLOC,
                 color, shade);
    }

  cairo_set_source_rgba (cr,
                         colors[color][shade][0],
                         colors[color][shade][1],
                         colors[color][shade][2],
                         alpha);
}

void
tango_gdk_set_color (GdkColor  * target,
                     TangoColor  source,
                     TangoShade  shade)
{
  g_return_if_fail (target != NULL);

  target->pixel = 0;
  target->red   = 0x0000;
  target->green = 0x0000;
  target->blue  = 0x0000;

  g_return_if_fail (/* 0 <= source &&*/ source < TANGO_N_COLORS);
  g_return_if_fail (/* 0 <= shade &&*/ shade < TANGO_N_SHADES);

  if (G_UNLIKELY (fixed[source][shade][0] == 0 && fixed[source][shade][1] == 0 && fixed[source][shade][2] == 0))
    {
      g_warning ("%s(%s): color %d and shade %d are not supported yet",
                 G_STRFUNC, G_STRLOC,
                 source, shade);
    }

  target->red   = fixed[source][shade][0];
  target->green = fixed[source][shade][1];
  target->blue  = fixed[source][shade][2];
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */

