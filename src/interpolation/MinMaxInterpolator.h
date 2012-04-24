// -*- c++ -*-

#ifndef MINMAXINTERPOLATOR_H
#define MINMAXINTERPOLATOR_H 1

#include "SingleParameterInterpolator.h"

namespace Interpolation {

class MinMaxInterpolator {
public:
    class Data {
    public:
        Data(SingleParameterInterpolator::Data& centerData)
                : mCenterData(centerData) { }

        virtual ~Data() { }

        SingleParameterInterpolator::Data& centerData()
            { return mCenterData; }

        const SingleParameterInterpolator::Data& centerData() const
            { return mCenterData; }

        virtual int duration() const
            { return mCenterData.duration(); }

        virtual Interpolation::SeriesData parameter(int time)
            { return mCenterData.parameter(time); }

        virtual void setInterpolated(int time, Quality q, float value)
            { mCenterData.setInterpolated(time, q, value); }

        virtual SeriesData minimum(int t) = 0;
        virtual SeriesData maximum(int t) = 0;
        virtual void setMinimum(int time, Quality q, float value) = 0;
        virtual void setMaximum(int time, Quality q, float value) = 0;
        virtual float fluctuationLevel() const = 0;

    private:
        SingleParameterInterpolator::Data& mCenterData;
    };

    Summary interpolate(Data& data, SingleParameterInterpolator& spi);
};

} // namespace Interpolation

#endif /* MINMAXINTERPOLATOR_H */
