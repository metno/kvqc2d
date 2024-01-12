#!/usr/bin/env python3

import re, sys

MISSING = -32767
NDAYS = 30
MAX_MISSING = 3

########################################################################
# parse text file with normal values

re_stationheader = re.compile(r'^TAM-.*normal for *([0-9]+) - ')
re_value = re.compile(r' *([0-9]+) +(-?[0-9]+(\.?[0-9]+))')

stations = {}
with open(sys.argv[1], 'r', encoding='ISO-8859-1') as normals_dat:
    for line in normals_dat:
        m = re_stationheader.match(line)
        if m:
            stationid = int(m.group(1))
            continue
        if not stationid in (7010, 46910, 70150, 76450, 86500, 93700, 96800):
            continue

        m = re_value.match(line)
        if not m:
            continue
        if not stationid in stations:
            stations[stationid] = [MISSING] * 365

        day = int(m.group(1))
        normal = float(m.group(2))

        stations[stationid][day-1] = normal

########################################################################
# write normals to execute SQL statements in unit tests

with open(sys.argv[2], 'w') as normals_cpp:
    print ("""

    #include "algorithms/TestDB.h"
    #include <gtest/gtest.h>
    #include <sstream>

    void prepare_daymeans_212(SqliteTestDB* db)
    {
    std::ostringstream sql;""", file=normals_cpp)
    for stationid in sorted(stations):
        for day0, normal in enumerate(stations[stationid]):
            if normal != MISSING:
                print ("sql << \"INSERT INTO qc2_statistical_reference_values VALUES(%5d,211,%3d,'ref_value',%.1f);\";" % (stationid, day0+1, normal), file=normals_cpp)
        print ("ASSERT_NO_THROW(db->exec(sql.str()));", file=normals_cpp)
    print ("""
    }

    // Local """ + """variables:
    // mode: c++
    // buffer-read-only: t
    // End:
    """, file=normals_cpp)
