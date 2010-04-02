#!/bin/bash

set -e

aclocal
autoconf

./configure $@
