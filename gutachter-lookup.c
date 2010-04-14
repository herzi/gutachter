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

#include "gutachter-lookup.h"

#include <string.h> /* strlen() */

guint32
gutachter_lookup_n_windows (void)
{
  GList  * windows = gtk_window_list_toplevels ();
  guint32  result = g_list_length (windows);

  g_list_free (windows);
  return result;
}

GtkWidget*
gutachter_lookup_widget (gchar const* path)
{
  GtkWidget  * result = NULL;
  GList      * window;
  GList      * windows;
  gchar const* lookup;
  gchar const* end;

  g_return_val_if_fail (path && *path, NULL);

  if (!g_str_has_prefix (path, "urn:"))
    {
      g_warning ("%s(%s): path is no URN: \"%s\" should start with \"urn:\"",
                 G_STRFUNC, G_STRLOC,
                 path);
      return NULL;
    }

  path += 4; /* path points to 32bit word boundary */

  if (!g_str_has_prefix (path, "gtk:"))
    {
      g_warning ("%s(%s): the URN doesn't match our namespace (it should now start with \"gtk\"): %s",
                 G_STRFUNC, G_STRLOC,
                 path);
      return NULL;
    }

  path += 4; /* path points to 32bit and 64bit word boundary */

  if (!g_str_has_prefix (path, "GtkWindow"))
    {
      g_warning ("the gtk namespace can only be used with GtkWindow functions");
      return NULL;
    }

  lookup = path + strlen ("GtkWindow");

  if (!g_str_has_prefix (lookup, "(\""))
    {
      g_warning ("GtkWindows can only be looked up by title right now (e.g. 'GtkWindow(\"window title\")'): %s",
                 path);
      return NULL;
    }

  lookup += 2;
  end = strstr (lookup, "\")");
  if (!end)
    {
      g_warning ("window title doesn't seem to be closed: %s", path);
      return NULL;
    }

  for (window = windows = gtk_window_list_toplevels (); window; window = window->next)
    {
      gchar const* title = gtk_window_get_title (window->data);

      if (g_str_has_prefix (lookup, title) && *(title + (end - lookup)) == '\0')
        {
          result = window->data;
        }
    }
  g_list_free (windows);

  return result;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
