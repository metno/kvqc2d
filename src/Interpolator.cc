
#include "Interpolator.h"

#include <kvalobs/kvData.h>
#include <kvalobs/kvDataOperations.h>

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

bool lt_Instrument::operator()(const Instrument& a, const Instrument& b) const
{
    if ( a.stationid != b.stationid )
        return a.stationid < b.stationid;
    if ( a.type != b.type )
        return a.type < b.type;
    if ( a.level != b.level )
        return a.level < b.level;
    if ( not kvalobs::compare::eq_sensor( a.sensor, b.sensor ) )
        return kvalobs::compare::lt_sensor( a.sensor, b.sensor );
    return a.paramid < b.paramid;
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
