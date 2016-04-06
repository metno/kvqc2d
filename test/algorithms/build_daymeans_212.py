#!/usr/bin/env python

import re, sys

MISSING = -32767
NDAYS = 30
MAX_MISSING = 3

########################################################################
# parse text file with normal values

re_stationheader = re.compile(r'^TAM-.*normal for *([0-9]+) - ')
re_value = re.compile(r' *([0-9]+) +(-?[0-9]+(\.?[0-9]+))')

normals_dat = open(sys.argv[1], 'r')

stations = {}
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
    if not stations.has_key(stationid):
        stations[stationid] = [MISSING] * 365
    
    day = int(m.group(1))
    normal = float(m.group(2))

    stations[stationid][day-1] = normal

normals_dat.close()

########################################################################
# write normals to execute SQL statements in unit tests

normals_cpp = open(sys.argv[2], 'w')
print >> normals_cpp, """

#include "algorithms/TestDB.h"
#include <gtest/gtest.h>
#include <sstream>

void prepare_daymeans_212(SqliteTestDB* db)
{
std::ostringstream sql;"""
for stationid in sorted(stations.iterkeys()):
    for day0, normal in enumerate(stations[stationid]):
        if normal != MISSING:
            print >> normals_cpp, "sql << \"INSERT INTO qc2_statistical_reference_values VALUES(%5d,211,%3d,'ref_value',%.1f);\";" % (stationid, day0+1, normal)
    print >> normals_cpp, "ASSERT_NO_THROW(db->exec(sql.str()));"
    # print >> normals_cpp, "try { db->exec(sql.str()); } catch(std::exception& ex) { FAIL() << ex.what(); }"
print >> normals_cpp, """
}

// Local """ + """variables:
// mode: c++
// buffer-read-only: t
// End:
"""
normals_cpp.close()
