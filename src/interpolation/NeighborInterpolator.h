/* -*- c++ -*-
 * NeighborInterpolator.h
 *
 *  Created on: Apr 19, 2012
 *      Author: alexanderb
 */

#ifndef NEIGHBORINTERPOLATION_H_
#define NEIGHBORINTERPOLATION_H_

#include "SingleParameterInterpolator.h"

namespace Interpolation {

class NeighborInterpolator : public SingleParameterInterpolator {
public:
    class Data: public SingleParameterInterpolator::Data {
    public:
        virtual int neighbors() const = 0;

        virtual float maximumOffset() const = 0;

        virtual SupportData model(int time) = 0;

        virtual SupportData transformedNeighbor(int n, int time) = 0;

        virtual float neighborWeight(int neighbor) = 0;
    };

    /** amount of extra data required before and after the gap for Akima interpolation. */
    static const int EXTRA_DATA;

    virtual Interpolation::Summary interpolate(SingleParameterInterpolator::Data& data);
    Interpolation::Summary doInterpolate(Data& data);

};

} // namespace Interpolation

#endif /* NEIGHBORINTERPOLATION_H_ */
