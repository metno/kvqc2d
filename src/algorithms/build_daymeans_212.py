#!/usr/bin/env python

import re, sys

re_stationheader = re.compile(r'^TAM-.*normal for *([0-9]+) - ')
re_value = re.compile(r' *([0-9]+) +(-?[0-9]+(\.?[0-9]+))')

normals_dat = open(sys.argv[1], 'r')
normals_cpp = open(sys.argv[2], 'w')

print >> normals_cpp, """
#ifndef StatisticalMean_H
#error "must be included after StatisticalMean.h"
#endif
"""

def listname(stationid):
    return "daymeans212_%05d" % stationid

def print_station():
    print >> normals_cpp, "static const int " + listname(stationids[-1]) + "[365] = { " + \
        (",").join(["%d" % d for d in daymeans]) + " };"

stationids = []
daymeans = [-32767] * 365

stationids = []
daymeans = [-32767] * 365
for line in normals_dat:
    m = re_stationheader.match(line)
    if m:
        stationid = int(m.group(1))
        continue
    if stationid < 1 or stationid > 99999:
        continue

    m = re_value.match(line)
    if not m:
        continue
    
    day = int(m.group(1))
    normal = float(m.group(2))

    if len(stationids) > 0 and stationids[-1] != stationid:
        print_station()
        daymeans = [-32767] * 365
    if len(stationids) == 0 or stationids[-1] != stationid:
        stationids.append(stationid)
    daymeans[day-1] = int(10*normal + 0.5)

print_station()

print >> normals_cpp, """
static const int daymeans212_n = """ + str(len(stationids)) + """;
static const int* daymeans212_list[daymeans212_n] = {""" + (",").join([listname(s) for s in stationids]) + """};
static const int daymeans212_ids [daymeans212_n] = {""" + (",").join(["%d" % s for s in stationids]) + """};

// Local """ + """variables:
// mode: c++
// buffer-read-only: t
// End:
"""

normals_cpp.close()
normals_dat.close()
