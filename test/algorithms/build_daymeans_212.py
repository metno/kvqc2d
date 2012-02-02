#!/usr/bin/env python

import re, sys

re_stationheader = re.compile(r'^TAM-.*normal for *([0-9]+) - ')
re_value = re.compile(r' *([0-9]+) +(-?[0-9]+(\.?[0-9]+))')

normals_dat = open(sys.argv[1], 'r')
normals_cpp = open(sys.argv[2], 'w')

print >> normals_cpp, "{\nstd::ostringstream sql;"

last_stationid = -1
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
    
    day = int(m.group(1))
    normal = float(m.group(2))

    if stationid != last_stationid and last_stationid >= 0:
        print >> normals_cpp, "ASSERT_NO_THROW(db->exec(sql.str()));\nsql.str("");"
    last_stationid = stationid
    print >> normals_cpp, "sql << \"INSERT INTO statistical_reference_values VALUES(%5d,212,%3d,'ref_value',%.1f);\";" % (stationid, day, normal)

if stationid != -1:
    print >> normals_cpp, "ASSERT_NO_THROW(db->exec(sql.str()));\nsql.str("");"

print >> normals_cpp, """
}

// Local """ + """variables:
// mode: c++
// buffer-read-only: t
// End:
"""

normals_cpp.close()
normals_dat.close()
