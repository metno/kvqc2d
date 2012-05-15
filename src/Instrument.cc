/* -*- c++ -*-
 Kvalobs - Free Quality Control Software for Meteorological Observations

 Copyright (C) 2011 met.no

 Contact information:
 Norwegian Meteorological Institute
 Postboks 43 Blindern
 N-0313 OSLO
 NORWAY
 email: kvalobs-dev@met.no

 This file is part of KVALOBS

 KVALOBS is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 KVALOBS is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License along
 with KVALOBS; if not, write to the Free Software Foundation Inc.,
 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Instrument.h"

#include <kvalobs/kvData.h>
#include <kvalobs/kvDataOperations.h>

Instrument::Instrument(int st, int pa, int se, int ty, int le)
        : stationid(st), paramid(pa), sensor(se), type(ty), level(le)
{
}

Instrument::Instrument(const kvalobs::kvData& d)
        : stationid(d.stationID()), paramid(d.paramID()), sensor(d.sensor()), type(d.typeID()), level(d.level())
{
}

bool lt_Instrument::operator()(const Instrument& a, const Instrument& b) const
{
    if (a.stationid != b.stationid)
        return a.stationid < b.stationid;
    if (a.type != b.type)
        return a.type < b.type;
    if (a.level != b.level)
        return a.level < b.level;
    if (not kvalobs::compare::eq_sensor(a.sensor, b.sensor))
        return kvalobs::compare::lt_sensor(a.sensor, b.sensor);
    return a.paramid < b.paramid;
}

std::ostream& operator<<(std::ostream& out, const Instrument& i)
{
    out << "[stationid=" << i.stationid << " AND paramid=" << i.paramid
        << " AND sensor='" << i.sensor << "' AND typeid=" << i.type << " AND level=" << i.level << "]";
    return out;
}
