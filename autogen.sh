#!/bin/bash

set -e

touch ChangeLog
aclocal
libtoolize --automake
automake --add-missing
autoconf

./configure $@
