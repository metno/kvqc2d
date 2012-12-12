#!/bin/sh

TP=kvqc2dTest
TD=test

set -e

make all
make -C $TD $TP
if test "$#" = 0; then
    FILTERS="*"
else
    FILTERS="$1"
    shift
    for f in "$@"; do
        FILTERS="$FILTERS:$f"
    done
fi
exec $TD/$TP  --gtest_print_time=1 --gtest_color=yes "--gtest_filter=$FILTERS"
