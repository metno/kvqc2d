
#include "Interpolator.h"

#include "AlgorithmConfig.h"
#include "Helpers.h"
#include "foreach.h"

#include <kvalobs/kvData.h>
#include <kvalobs/kvDataOperations.h>

#define NDEBUG 1
#include "debug.h"

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

// ========================================================================

void BasicParameterInfo::constrain(float& value) const
{
    if( minValue != Interpolator::INVALID && value < minValue )
        value = minValue;
    if( maxValue != Interpolator::INVALID && value > maxValue )
        value = maxValue;
}

// ------------------------------------------------------------------------

BasicParameterInfo::BasicParameterInfo(const std::string& pi)
  : parameter(-1)
  , minValue(Interpolator::INVALID)
  , maxValue(Interpolator::INVALID)
{
    const Helpers::splitN_t items = Helpers::splitN(pi, ",", true);
    foreach(const std::string& item, items) {
        Helpers::split2_t kv = Helpers::split2(item, "=");
        if( kv.first == "par" ) {
            parameter = std::atoi(kv.second.c_str());
        } else if( kv.first == "minVal" ) {
            minValue = std::atof(kv.second.c_str());
        } else if( kv.first == "maxVal" ) {
            maxValue = std::atof(kv.second.c_str());
        }
    }
}

// ------------------------------------------------------------------------

ParameterInfo::ParameterInfo(const std::string& pi)
    : BasicParameterInfo(pi)
    , minParameter(-1)
    , maxParameter(-1)
{
    const Helpers::splitN_t items = Helpers::splitN(pi, ",", true);
    foreach(const std::string& item, items) {
        Helpers::split2_t kv = Helpers::split2(item, "=");
        if( kv.first == "minPar" ) {
            minParameter = std::atoi(kv.second.c_str());
        } else if( kv.first == "maxPar" ) {
            maxParameter = std::atoi(kv.second.c_str());
        } else if( kv.first == "fluctuationLevel" ) {
            fluctuationLevel = std::atof(kv.second.c_str());
        }
    }
}
