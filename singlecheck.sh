#!/bin/sh

TP=kvqc2dTest
TD=test

set -e

make all
make -C $TD $TP
$TD/$TP –gtest_color=yes --gtest_filter="${1:-*}"
