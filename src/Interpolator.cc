
#include "Interpolator.h"

#include <kvalobs/kvData.h>

Interpolator::~Interpolator()
{
}

// ------------------------------------------------------------------------

Interpolator::ValuesWithQualities_t Interpolator::interpolate(const kvalobs::kvData& before, const miutil::miTime& afterGap)
{
    return interpolate(before.obstime(), afterGap, before.stationID(), before.paramID());
}
