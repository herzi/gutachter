#!/bin/bash

set -e

touch ChangeLog
aclocal
automake --add-missing
autoconf

./configure $@
