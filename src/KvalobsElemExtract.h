/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011-2012 met.no

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

#ifndef KvalobsElemExtract_h
#define KvalobsElemExtract_h 1

#include "KvalobsDbGate.h"

/**
 * \brief Template to extract something from a dnmi::db::DRow and insert it into
 * a container.
 */
template<class Elem, class Iterator>
class KvalobsElemExtract : public KvalobsDbExtract {
public:
    KvalobsElemExtract(Iterator push)
        : mPush(push) { }

    void extractFromRow(const dnmi::db::DRow& row)
        { *mPush++ = Elem(row); }

private:
    Iterator mPush;
};

template<class E, class I>
KvalobsElemExtract<E, I>* makeElementExtract(I i)
{
    return new KvalobsElemExtract<E, I>(i);
}

#endif /* KvalobsElemExtract_h */
