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

#ifndef DEBUG_H
#define DEBUG_H 1

#include <milog/milog.h>
#include <iostream>

#ifndef NDEBUG
#define DBGINF(x) std::cout << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]\n    " << x << std::endl
#define DBG(x) do { LOGDEBUG(x); DBGINF(x); } while(false);
#define INF(x) do { LOGINFO(x);  DBGINF(x); } while(false);
#define DBGL std::cout << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]" << std::endl
#else
#define DBG(x) do { /* nothing */ } while(false);
#define INF(x) do { LOGINFO(x); } while(false);
#define DBGL DBG("");
#endif
#define DBG1(x) " " #x "='" << x << "'"
#define DBGV(x) DBG( DBG1(x) )

#endif /* DEBUG_H */
