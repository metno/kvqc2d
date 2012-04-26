// -*- c++ -*-

#ifndef SINGLEPARAMETERINTERPOLATOR_H
#define SINGLEPARAMETERINTERPOLATOR_H 1

#include "InterpolationData.h"
#include "SimpleInterpolationResult.h"

namespace Interpolation {

class SingleParameterInterpolator {
public:
    class Data {
    public:
        virtual ~Data();

        virtual int duration() = 0;

        virtual SeriesData parameter(int time) = 0;

        virtual void setInterpolated(int time, Quality q, float value) = 0;

        virtual SimpleResult getInterpolated(int time) = 0;
    };

    virtual ~SingleParameterInterpolator();

    virtual Summary interpolate(Data& data) = 0;
};

} // namespace Interpolation

#endif /* SINGLEPARAMETERINTERPOLATOR_H */
