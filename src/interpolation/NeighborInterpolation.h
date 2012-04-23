/* -*- c++ -*-
 * NeighborInterpolator.h
 *
 *  Created on: Apr 19, 2012
 *      Author: alexanderb
 */

#ifndef NEIGHBORINTERPOLATION_H_
#define NEIGHBORINTERPOLATION_H_

#include <vector>

namespace NeighborInterpolation {

class SupportData {
public:
    SupportData() : mUsable(false), mValue(-32766.5) { }
    SupportData(float v) : mUsable(true), mValue(v) { }
    bool usable() const { return mUsable; }
    float value() const { return mValue; }
private:
    bool mUsable;
    float mValue;
};

class SeriesData : public SupportData {
public:
    SeriesData() : SupportData(), mNeedsInterpolation(true) { }
    SeriesData(float v) : SupportData(v), mNeedsInterpolation(false) { }
    explicit SeriesData(const SupportData& sd) : SupportData(sd), mNeedsInterpolation(!sd.usable()) { }
    bool needsInterpolation() const { return mNeedsInterpolation; }
private:
    bool mNeedsInterpolation;
};

enum Quality { GOOD, BAD, FAILED };

class Data {
public:
    virtual ~Data();

    virtual int duration() const = 0;

    virtual int neighbors() const = 0;

    virtual float maximumOffset() const = 0;

    virtual SeriesData center(int time) = 0;

    virtual SupportData model(int time) = 0;

    virtual SupportData transformedNeighbor(int n, int time) = 0;

    virtual float neighborWeight(int neighbor) = 0;

    virtual void setInterpolated(int time, Quality q, float value) = 0;
};

/** amount of extra data required before and after the gap for Akima interpolation. */
extern const int extraData;

void interpolate(Data& data);

} // namespace NeighborInterpolator

#endif /* NEIGHBORINTERPOLATION_H_ */
