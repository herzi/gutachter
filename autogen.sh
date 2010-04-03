#!/bin/bash

set -e

touch ChangeLog
aclocal
libtoolize --automake
intltoolize --automake
automake --add-missing
autoconf

./configure $@
