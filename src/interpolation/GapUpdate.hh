
#ifndef GAPUPDATE_HH
#define GAPUPDATE_HH 1

#include "algorithms/DataUpdate.h"
#include "interpolation/InterpolationData.h"

class GapUpdate : public DataUpdate {
public:
    GapUpdate();

    GapUpdate(const kvalobs::kvData& data, Interpolation::Quality q, float value, bool usable);

    float value() const
        { return mValue; }

    float original() const
        { return mOriginalValue; }

    Interpolation::Quality quality() const
        { return mQuality; }

    bool usable() const
        { return mUsable; }

    bool isUpdated() const
        { return mUpdated; }

    bool update(Interpolation::Quality q, float value, bool allowWorse=false);

private:
    Interpolation::Quality mQuality;
    float mValue;
    float mOriginalValue;
    bool mUpdated;
    bool mUsable;
};


#endif /* GAPUPDATE_HH */
