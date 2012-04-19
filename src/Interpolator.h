// -*- c++ -*-

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H 1

#include "TimeRange.h"
#include <kvalobs/kvData.h>
#include <puTools/miTime.h>
#include <vector>

class AlgorithmConfig;

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

struct BasicParameterInfo {
    /** observed parameter id */
    int parameter;

    /** minimum physical value */
    float minValue;

    /** maximum physical value */
    float maxValue;

    /** force value is inside physical value range */
    void constrain(float& value) const;

    /** return value forced inside physical value range */
    float constrained(float value) const
        { constrain(value); return value; }

    BasicParameterInfo(const std::string& info);
};

struct ParameterInfo : public BasicParameterInfo {
    /** parameter id for minimum of 'parameter' between observations */
    int minParameter;

    /** parameter id for maximum of 'parameter' between observations */
    int maxParameter;

    /** approximate fluctuation of the parameter while measuring min
     * and max between two observations */
    float fluctuationLevel;

    ParameterInfo(const std::string& info);
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
