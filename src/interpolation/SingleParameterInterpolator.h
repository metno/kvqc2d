// -*- c++ -*-

#ifndef SINGLEPARAMETERINTERPOLATOR_H
#define SINGLEPARAMETERINTERPOLATOR_H 1

#include "InterpolationData.h"

namespace Interpolation {

class SingleParameterInterpolator {
public:
    class Data {
    public:
        virtual ~Data();

        virtual int duration() const = 0;

        virtual SeriesData parameter(int time) = 0;

        virtual void setInterpolated(int time, Quality q, float value) = 0;
    };

    virtual ~SingleParameterInterpolator();

    virtual Summary interpolate(Data& data) = 0;
};

} // namespace Interpolation

#endif /* SINGLEPARAMETERINTERPOLATOR_H */
