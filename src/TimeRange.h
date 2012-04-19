// -*- c++ -*-

#ifndef TIMERANGE_H
#define TIMERANGE_H 1

#include <puTools/miTime.h>

struct TimeRange {
    miutil::miTime t0;
    miutil::miTime t1;
    TimeRange(const miutil::miTime& T0, const miutil::miTime& T1)
        : t0(T0), t1(T1) { }
    int days() const;
    int hours() const;
};

#endif /* TIMERANGE_H */
