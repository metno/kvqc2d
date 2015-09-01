// -*- c++ -*-

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H 1

#include <puTools/miTime.h>
#include <vector>

namespace kvalobs {
class kvData;
}
class AlgorithmConfig;

// ========================================================================
    
struct TimeRange {
    miutil::miTime t0;
    miutil::miTime t1;
    TimeRange(const miutil::miTime& T0, const miutil::miTime& T1)
        : t0(T0), t1(T1) { }
    int days() const;
    int hours() const;
};

// ========================================================================

struct Instrument {
    int stationid;
    int paramid;
    int sensor;
    int type;
    int level;
    Instrument(int st, int pa, int se, int ty, int le)
        : stationid(st), paramid(pa), sensor(se), type(ty), level(le) { }
};
    
// ========================================================================

class Interpolator {
public:
    virtual ~Interpolator();

    struct ValueWithQuality {
        float value;
        int quality;
        ValueWithQuality(float v, int q)
            : value(v), quality(q) { }
    };

    typedef std::vector<ValueWithQuality> ValuesWithQualities_t;

    virtual ValuesWithQualities_t interpolate(const Instrument& instrument, const TimeRange& t) = 0;

    ValuesWithQualities_t interpolate(const kvalobs::kvData& before, const miutil::miTime& after);

    virtual void configure(const AlgorithmConfig& config) = 0;
};

#endif /* INTERPOLATOR_H */