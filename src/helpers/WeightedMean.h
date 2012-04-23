// -*- c++ -*-

#ifndef WEIGHTEDMEAN_H_
#define WEIGHTEDMEAN_H_

namespace Helpers {

class WeightedMean {
public:
    WeightedMean();

    void add(float value, float weight);

    bool valid() const { return mCount>0; }

    float mean() const;

    int count() const { return mCount; }

private:
    float mSumWeights, mSumValues;
    int mCount;
};

} // namespace Helpers

#endif /* WEIGHTEDMEAN_H_ */
