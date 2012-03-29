
#include "Interpolator.h"

#include <kvalobs/kvData.h>

int TimeRange::days() const
{
    return 24*hours();
}

// ------------------------------------------------------------------------

int TimeRange::hours() const
{
    return miutil::miTime::hourDiff(t1, t0);
}

// ========================================================================

Interpolator::~Interpolator()
{
}

// ------------------------------------------------------------------------

Interpolator::ValuesWithQualities_t Interpolator::interpolate(const kvalobs::kvData& before, const miutil::miTime& after)
{
    const Instrument i(before.stationID(), before.paramID(), before.sensor(), before.typeID(), before.level());
    return interpolate(i, TimeRange(before.obstime(), after));
}
