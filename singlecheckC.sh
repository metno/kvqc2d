#!/bin/sh

TP=kvqc2dTest
TD=test
B=build

set -e

test -d $B || ( mkdir -p $B && cd $B && cmake .. )
make -C $B

if test "$#" = 0; then
    FILTERS="*"
else
    FILTERS="$1"
    shift
    for f in "$@"; do
        FILTERS="$FILTERS:$f"
    done
fi
exec $B/$TD/$TP  --gtest_print_time=1 --gtest_color=yes "--gtest_filter=$FILTERS"
