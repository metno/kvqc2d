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

#ifndef KVALOBSDATALIST_H_
#define KVALOBSDATALIST_H_

#include "InterpolationData.h"
#include "ParameterInfo.h"
#include "foreach.h"
#include <list>

template<class KvalobsData, class InterpolationData>
class KvalobsDataList {
public:
    KvalobsDataList()
        : mFetchRequired(true) { }

    InterpolationData find(const miutil::miTime& t, const ParameterInfo& pi) const;
    bool fetchRequired();
    void set(const std::list<KvalobsData>& data);

private:
    bool mFetchRequired;
    std::list<KvalobsData> mData;
};

template<class KvalobsData, class InterpolationData>
InterpolationData KvalobsDataList<KvalobsData, InterpolationData>::find(const miutil::miTime& t, const ParameterInfo& pi) const
{
    foreach(const KvalobsData& d, mData) {
        if( d.obstime() == t ) {
            const float storage = d.original();
            if( pi.hasNumerical(storage) )
                return InterpolationData(pi.toNumerical(storage));
            else
                return InterpolationData();
        }
    }
    return InterpolationData();
}

template<class KvalobsData, class InterpolationData>
bool KvalobsDataList<KvalobsData, InterpolationData>::fetchRequired()
{
    const bool f = mFetchRequired;
    mFetchRequired = false;
    return f;
}

template<class KvalobsData, class InterpolationData>
void KvalobsDataList<KvalobsData, InterpolationData>::set(const std::list<KvalobsData>& data)
{
    mData = data;
}

typedef KvalobsDataList<kvalobs::kvData, Interpolation::SeriesData> KvalobsSeriesDataList;
typedef KvalobsDataList<kvalobs::kvData, Interpolation::SupportData> KvalobsSupportDataList;
typedef KvalobsDataList<kvalobs::kvModelData, Interpolation::SupportData> KvalobsSupportModelDataList;

#endif /* KVALOBSDATALIST_H_ */
