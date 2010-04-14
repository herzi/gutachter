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

#include <stdlib.h> /* strtol() */
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
  GList      * list;
  GList      * iterator;
  gchar const* lookup;
  gchar const* end;

  g_return_val_if_fail (path && *path, NULL);

  lookup = path;

  if (!g_str_has_prefix (lookup, "urn:"))
    {
      g_warning ("%s(%s): path is no URN: \"%s\" should start with \"urn:\"",
                 G_STRFUNC, G_STRLOC,
                 path);
      return NULL;
    }

  lookup += 4; /* path points to 32bit word boundary */

  if (!g_str_has_prefix (lookup, "gtk:"))
    {
      g_warning ("%s(%s): the URN's namespace doesn't match ours (\"gtk\"): %s",
                 G_STRFUNC, G_STRLOC,
                 path);
      return NULL;
    }

  lookup += 4; /* path points to 32bit and 64bit word boundary */

  if (!g_str_has_prefix (lookup, "GtkWindow"))
    {
      g_warning ("the gtk namespace can only be used with GtkWindow functions");
      return NULL;
    }

  lookup += strlen ("GtkWindow");

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

  for (iterator = list = gtk_window_list_toplevels (); iterator; iterator = iterator->next)
    {
      gchar const* title = gtk_window_get_title (iterator->data);

      if (g_str_has_prefix (lookup, title) && *(title + (end - lookup)) == '\0')
        {
          result = iterator->data;
          break;
        }
    }
  g_list_free (list);

  lookup = end + 2; /* result holds the window now */

  while (*lookup != '\0')
    {
      gchar* type_name;
      GType  type;
      int    index;

      if (*lookup != ':')
        {
          g_warning ("unexpected character after widget lookup (column %d): expected dereferencing (\":\") or end: '%c'",
                     lookup - path, *lookup);
          return NULL;
        }
      lookup++;

      end = strstr (lookup, "[");
      if (!end)
        {
          g_warning ("the type starting at column %d doesn't have a lookup operator (\"[]\")",
                     lookup - path);
          return NULL;
        }

      type_name = g_strndup (lookup, end - lookup);
      type = g_type_from_name (type_name);
      if (!type)
        {
          g_warning ("couldn't lookup the type \"%s\". it is not registered. this "
                     "usually means that you have a typo in your string as the "
                     "creation of a widget would automatically register its type "
                     "(and its parent types)", type_name);
          g_free (type_name);
          return NULL;
        }
      g_free (type_name);

      lookup = end + 1;
      index = strtol (lookup, &type_name, 10); /* FIXME: watch for ERANGE in errno */
      end = type_name; /* use type_name because it is non-const */

      if (!end || *end != ']')
        {
          g_warning ("the index starting at column %d doesn't seem to be properly terminated: expected ']', got '%c': %s",
                     lookup - path, *end,
                     path);
          return NULL;
        }

      if (!GTK_IS_CONTAINER (result))
        {
          gchar* result_end = g_strrstr_len (path, lookup - path, ":");
          gchar* result_path = g_strndup (path, result_end - path);
          g_warning ("the widget specified by \"%s\" is a %s (which is not a GtkContainer)",
                     result_path,
                     G_OBJECT_TYPE_NAME (result));
          g_free (result_path);
          return NULL;
        }

      list = gtk_container_get_children (GTK_CONTAINER (result));
      iterator = g_list_nth (list, index);
      if (!iterator)
        {
          g_warning ("%s doesn't have a child with the index %d",
                     G_OBJECT_TYPE_NAME (result), index);
          g_list_free (list);
          return NULL;
        }
      result = iterator->data;
      g_list_free (list);

      lookup = end + 1;
    }

  return result;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
