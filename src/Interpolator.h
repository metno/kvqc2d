// -*- c++ -*-

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H 1

#include <kvalobs/kvData.h>
#include <puTools/miTime.h>
#include <vector>

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
    explicit Instrument(const kvalobs::kvData& d)
        : stationid(d.stationID()), paramid(d.paramID()), sensor(d.sensor()), type(d.typeID()), level(d.level()) { }
};

struct lt_Instrument : public std::binary_function<Instrument, Instrument, bool>
{
    bool operator()(const Instrument& a, const Instrument& b) const;
};

// ========================================================================

class Interpolator {
public:
    enum { INVALID = -32767 };

    virtual ~Interpolator();

    enum { QUALITY_OBS, QUALITY_INTER_GOOD, QUALITY_INTER_BAD, QUALITY_INTER_FAILED };

    struct ValueWithQuality {
        float value;
        int quality;
        ValueWithQuality(float v, int q)
            : value(v), quality(q) { }
        bool valid() const
            { return quality != QUALITY_INTER_FAILED && value != INVALID; }
    };

    typedef std::vector<ValueWithQuality> ValuesWithQualities_t;

    virtual ValuesWithQualities_t interpolate(const Instrument& instrument, const TimeRange& t) = 0;

    ValuesWithQualities_t interpolate(const kvalobs::kvData& before, const miutil::miTime& after);

    virtual void configure(const AlgorithmConfig& config) = 0;
};

#endif /* INTERPOLATOR_H */
