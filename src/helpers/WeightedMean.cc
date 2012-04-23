/*
 * WeightedMean.cc
 *
 *  Created on: Apr 23, 2012
 *      Author: alexanderb
 */

#include "WeightedMean.h"

namespace Helpers {

WeightedMean::WeightedMean()
: mSumWeights(0), mSumValues(0), mCount(0)
{
}

void WeightedMean::add(float value, float weight)
{
    if(weight>0) {
        mSumValues  += value*weight;
        mSumWeights += weight;
        mCount += 1;
    }
}

float WeightedMean::mean() const
{
    if( valid() )
        return mSumValues / mSumWeights;
    else
        return 0;
}

} // namespace Helpers
