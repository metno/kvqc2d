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

#ifndef DBCONSTRAINTSBASE_H_
#define DBCONSTRAINTSBASE_H_

#include <boost/shared_ptr.hpp>
#include <string>

template<class T>
class SQLBuilderPointer {
public:
    template<class D>
    SQLBuilderPointer(const SQLBuilderPointer<D>& d)
        : p(d.p) { }

    explicit SQLBuilderPointer()
        : p(new T()) { }

    template<class P1>
    explicit SQLBuilderPointer(const P1& p1)
        : p(new T(p1)) { }

    template<class P1, class P2>
    explicit SQLBuilderPointer(const P1& p1, const P2& p2)
        : p(new T(p1, p2)) { }

    template<class P1, class P2, class P3>
    explicit SQLBuilderPointer(const P1& p1, const P2& p2, const P3& p3)
        : p(new T(p1, p2, p3)) { }

    template<class P1, class P2, class P3, class P4>
    explicit SQLBuilderPointer(const P1& p1, const P2& p2, const P3& p3, const P4& p4)
        : p(new T(p1, p2, p3, p4)) { }

    std::string sql() const
        { return p->sql(); }

protected:
    template<class D> friend class SQLBuilderPointer;

    boost::shared_ptr<T> p;
};

namespace Constraint {

class DBConstraintImpl  {
public:
    virtual ~DBConstraintImpl() { }
    virtual std::string sql() const = 0;
};
typedef SQLBuilderPointer<DBConstraintImpl> DBConstraint;

} // namespace Constraint

namespace Ordering {

class DBOrderingImpl {
public:
    virtual ~DBOrderingImpl() { }
    virtual std::string sql() const = 0;
};
typedef SQLBuilderPointer<DBOrderingImpl> DBOrdering;

} // namespace Ordering

#endif /* DBCONSTRAINTSBASE_H_ */
