#!/bin/bash

set -e

touch ChangeLog
aclocal
libtoolize --automake
intltoolize --automake
gtkdocize --flavour no-tmpl
automake --add-missing
autoconf

./configure --enable-gtk-doc $@
