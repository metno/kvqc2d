#!/bin/sh

TP=kvqc2dTest
TD=test

set -e

make all
make -C $TD $TP
$TD/$TP  --gtest_print_time=1 --gtest_color=yes --gtest_filter="${1:-*}"
