#! /bin/bash

grep -Eo 'VERSION +[0-9]+\.[0-9]+\.[0-9]+([^ ]+)?' CMakeLists.txt \
    | sed -E 's/.*VERSION +//'



