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

#ifndef FOREACH_H_
#define FOREACH_H_

// this is just to help the eclipse cdt parser "understand" BOOST_FOREACH

#ifdef __CDT_PARSER__
#define foreach(a, b) for(a : b)
#define foreach_r(a, b) for(a : b)
#error "foreach_r is defined wrongly for the cdt parser"
#else

#ifdef __GNUG__

namespace gcc_foreach_helpers {
template<typename T, bool>
struct iterator_helper;

template<typename T>
struct iterator_helper<T, false> {
    typedef typename T::iterator         iforward;
    typedef typename T::reverse_iterator ireverse;
};

template<typename T>
struct iterator_helper<T, true> {
    typedef typename T::const_iterator         iforward;
    typedef typename T::const_reverse_iterator ireverse;
};

// this is a simplified version of boost/type_traits/is_const which probably works for fewer types
template<class T>
int  is_const_or_not(const T*);

template<class T>
char is_const_or_not(T*);

template<typename T>
struct is_const {
    static T* t;
    static const bool value = (sizeof(is_const_or_not(t)) != 1);
};

}

#define GCC_FOREACH_BASE( i, c, BEGIN, END, ITER )                      \
    for(bool continu=true; continu; continu=false)                      \
        for(__typeof__( c )& c_REF = (c); continu; continu = false)     \
            for( gcc_foreach_helpers::iterator_helper<__typeof__( c ),gcc_foreach_helpers::is_const<__typeof__( c )>::value>::ITER c_ITERATOR = c_REF.BEGIN(); \
                 continu && c_ITERATOR != c_REF.END();                  \
                 ++ c_ITERATOR, continu = !continu )                    \
                for(i = *c_ITERATOR; continu; continu=!continu)
#define foreach(a, b)   GCC_FOREACH_BASE(a, b, begin,  end,  iforward)
#define foreach_r(a, b) GCC_FOREACH_BASE(a, b, rbegin, rend, ireverse)

#else

#include <boost/foreach.hpp>
// FIXME defining foreach like this is not good, according to http://www.boost.org/doc/libs/1_40_0/doc/html/foreach.html (bottom)
#define foreach(a, b) BOOST_FOREACH(a, b)
#define foreach_r(a, b) BOOST_REVERSE_FOREACH(a, b)

#endif
#endif

#endif /* FOREACH_H_ */
