
#include "GapUpdate.hh"

#include "gdebug.h"

GapUpdate::GapUpdate()
    : DataUpdate()
    , mQuality(Interpolation::NO_ROW)
    , mValue(Interpolation::MISSING_VALUE)
    , mOriginalValue(mValue)
    , mUpdated(false)
    , mUsable(false)
{
}

GapUpdate::GapUpdate(const kvalobs::kvData& data, Interpolation::Quality q, float value, bool usable)
    : DataUpdate(data)
    , mQuality(q)
    , mValue(value)
    , mOriginalValue(mValue)
    , mUpdated(false)
    , mUsable(usable)
{
}

bool GapUpdate::update(Interpolation::Quality quality, float value, bool allowWorse)
{
    if( mQuality == Interpolation::NO_ROW and quality >= Interpolation::FAILED )
        return false;
    DBG(DBG1(*this) << DBG1(quality) << DBG1(value) << DBG1(allowWorse) << DBG1(mQuality));
    if( allowWorse or (quality < mQuality) ) {
        mQuality = quality;
        mValue = value;
        mUpdated = true;
        mUsable = mQuality < Interpolation::FAILED;
        DBG(DBG1(*this) << DBG1(mQuality) << DBG1(mValue) << DBG1(mUsable));
        return true;
    } else {
        return false;
    }
}
