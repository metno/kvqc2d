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
# transform to arithmetic mean of the last 30 days
for stationid in sorted(stations.iterkeys()):
    normals_file = stations[stationid]
    normals = [MISSING] * 365
    for day0 in range(365):
        day1 = day0 + 1
        if day0 < NDAYS:
            values = normals_file[365+day1-NDAYS:365] + normals_file[:day1]
        else:
            values = normals_file[day1-NDAYS:day1]
        n_missing = values.count(MISSING)
        if n_missing > MAX_MISSING:
            print "no normal for station=%5d day%3d" % (stationid, day1)
            continue
        values = filter(lambda v: v != MISSING, values)
        normals[day0] = sum(values) / len(values)
    stations[stationid] = normals

########################################################################
# write normals to execute SQL statements in unit tests

normals_cpp = open(sys.argv[2], 'w')
print >> normals_cpp, "{\nstd::ostringstream sql;"
for stationid in sorted(stations.iterkeys()):
    for day0, normal in enumerate(stations[stationid]):
        if normal != MISSING:
            print >> normals_cpp, "sql << \"INSERT INTO statistical_reference_values VALUES(%5d,211,%3d,'ref_value',%.1f);\";" % (stationid, day0+1, normal)
    print >> normals_cpp, "ASSERT_NO_THROW(db->exec(sql.str()));\nsql.str(\"\");"
print >> normals_cpp, """
}

// Local """ + """variables:
// mode: c++
// buffer-read-only: t
// End:
"""
normals_cpp.close()
