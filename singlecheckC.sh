#!/bin/sh

TP=kvqc2dTest
TD=test
B=build

set -e

test -d $B || ( mkdir -p $B && cd $B && cmake .. )
make -C $B
./$B/$TD/$TP  --gtest_print_time=1 --gtest_color=yes --gtest_filter="${1:-*}"
