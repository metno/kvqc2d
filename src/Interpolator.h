// -*- c++ -*-

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H 1

#include <vector>

namespace miutil {
class miTime;
}
namespace kvalobs {
class kvData;
}
class AlgorithmConfig;

// ------------------------------------------------------------------------

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

    virtual ValuesWithQualities_t interpolate(const miutil::miTime& beforeGap, const miutil::miTime& afterGap,
                                              int stationid, int paramid) = 0;

    ValuesWithQualities_t interpolate(const kvalobs::kvData& before, const miutil::miTime& afterGap);

    virtual void configure(const AlgorithmConfig& config) = 0;
};

#endif /* INTERPOLATOR_H */
