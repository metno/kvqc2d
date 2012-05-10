/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 Copyright (C) 2011-2012 met.no

 Contact information:
 Norwegian Meteorological Institute
 Box 43 Blindern
 0313 OSLO
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

#include "StationParamParser.h"

#include "helpers/stringutil.h"

#include "foreach.h"

StationParamParser::StationParamParser(const kvalobs::kvStationParam& sp)
{
    // see https://kvalobs.wiki.met.no/doku.php?id=kvalobs:tabeller_i_kvalobsdatabasen_forklaring&s[]=metadata table station_param
    Helpers::split2_t kv = Helpers::split2(sp.metadata(), "\n", true);

    const std::vector<std::string> keys = Helpers::splitN(kv.first, ";", true),
            values = Helpers::splitN(kv.second, ";", true);
    for(size_t j=0; j<keys.size(); j++)
        mMetadata[ keys[j] ] = values[j];
}

float StationParamParser::floatValue(const std::string& key, float dflt) const
{
    const MetaDataMap::const_iterator it = mMetadata.find(key);
    if( it == mMetadata.end() )
        return dflt;
    return std::atof(it->second.c_str());
}

