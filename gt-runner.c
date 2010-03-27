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

#include "gt-runner.h"

static void
gtk_test_runner_iface_init (GtkTestRunnerIface* iface)
{
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("test-suite", NULL, NULL,
                                                            GTK_TEST_TYPE_SUITE, G_PARAM_READABLE));
}

GType
gtk_test_runner_get_type (void)
{
  static GType  stored_type = 0;

  if (g_once_init_enter (&stored_type))
    {
      GType  registered_type = g_type_register_static_simple (G_TYPE_INTERFACE,
                                                              "GtkTestRunner",
                                                              sizeof (GtkTestRunnerIface),
                                                              (GClassInitFunc) gtk_test_runner_iface_init,
                                                              0, NULL, 0);

      g_once_init_leave (&stored_type, registered_type);
    }

  return stored_type;
}

GFile*
gtk_test_runner_get_file (GtkTestRunner* self)
{
  GtkTestRunnerIface* iface;

  g_return_val_if_fail (GTK_TEST_IS_RUNNER (self), NULL);

  iface = GTK_TEST_RUNNER_GET_IFACE (self);
  if (!iface->get_file)
    {
      g_warning ("%s(%s): %s doesn't implement gtk_test_runner_get_file()",
                 G_STRFUNC, G_STRLOC, G_OBJECT_TYPE_NAME (self));
      return NULL;
    }

  return iface->get_file (self);
}

GtkTestSuite*
gtk_test_runner_get_suite (GtkTestRunner* self)
{
  GtkTestRunnerIface* iface;

  g_return_val_if_fail (GTK_TEST_IS_RUNNER (self), NULL);

  iface = GTK_TEST_RUNNER_GET_IFACE (self);
  if (!iface->get_suite)
    {
      g_warning ("%s(%s): %s doesn't implement gtk_test_runner_get_suite()",
                 G_STRFUNC, G_STRLOC, G_OBJECT_TYPE_NAME (self));
      return NULL;
    }

  return iface->get_suite (self);
}

void
gtk_test_runner_set_file (GtkTestRunner* self,
                          GFile        * file)
{
  GtkTestRunnerIface* iface;

  g_return_if_fail (GTK_TEST_IS_RUNNER (self));

  iface = GTK_TEST_RUNNER_GET_IFACE (self);
  if (!iface->set_file)
    {
      g_warning ("%s(%s): %s doesn't implement gtk_test_runner_set_file()",
                 G_STRFUNC, G_STRLOC, G_OBJECT_TYPE_NAME (self));
    }
  else
    {
      iface->set_file (self, file);
    }
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
