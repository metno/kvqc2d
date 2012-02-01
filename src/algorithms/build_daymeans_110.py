#!/usr/bin/env python

import re, sys

normals_dat = open(sys.argv[1], 'r')
normals_cpp = open(sys.argv[2], 'w')

print >> normals_cpp, """
#ifndef StatisticalMean_H
#error "must be included after StatisticalMean.h"
#endif
"""

def listname(stationid):
    return "daymeans110_%05d" % stationid

def print_station():
    print >> normals_cpp, "static const int " + listname(stationids[-1]) + "[365] = { " + \
        (",").join(["%d" % d for d in daymeans]) + " };"

stationids = []
daymeans = [-32767] * 365
for line in normals_dat:
    stationid, date, day, normal, years = line.split()
    stationid = int(stationid)
    day = int(day)
    normal = float(normal)

    if stationid < 1 or stationid > 99999:
        continue
    if len(stationids) > 0 and stationids[-1] != stationid:
        print_station()
        daymeans = [-32767] * 365
    if len(stationids) == 0 or stationids[-1] != stationid:
        stationids.append(stationid)
    daymeans[day-1] = int(10*normal + 0.5)

print_station()

print >> normals_cpp, """
static const int daymeans110_n = """ + str(len(stationids)) + """;
static const int* daymeans110_list[daymeans110_n] = {""" + (",").join([listname(s) for s in stationids]) + """};
static const int daymeans110_ids [daymeans110_n] = {""" + (",").join(["%d" % s for s in stationids]) + """};

// Local """ + """variables:
// mode: c++
// buffer-read-only: t
// End:
"""

normals_cpp.close()
normals_dat.close()
