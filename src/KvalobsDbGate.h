/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2012 met.no

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

#ifndef KvalobsDbGate_H
#define KvalobsDbGate_H 1

#include <kvalobs/kvDbBase.h>
#include <kvdb/kvdb.h>

#include <list>

class KvalobsDbExtract {
public:
    virtual ~KvalobsDbExtract();
    virtual void extractFromRow(const dnmi::db::DRow& row) = 0;
};

// ========================================================================

class KvalobsDbGate  {
public:
    typedef dnmi::db::Connection Connection_t;
    typedef enum { INSERTONLY, INSERT_OR_UPDATE, INSERT_OR_REPLACE} InsertOption;

public:
    KvalobsDbGate();

    KvalobsDbGate(Connection_t *c);

    ~KvalobsDbGate();

    void setBusyTimeout(int timeoutInSeconds)
        { mBusyTimeout = timeoutInSeconds; }

    int getBusyTimeout() const
        { return mBusyTimeout; }

    void setConnection(Connection_t *c)
        { mConnection = c; }

    Connection_t* getConnection() const
        { return mConnection; }

#if 0
   template<class T>
   bool insert(const std::list<T>& li , bool replace=false,
               const miutil::miString &tblName="")
   {
      typename std::list<T>::const_iterator it=li.begin();
      std::list<kvalobs::kvDbBase*> myList;

      if(it==li.end())
         return true;

      std::string tbl;

      if(tblName.empty())
         tbl=it->tableName();
      else
         tbl=tblName;


      for(;it!=li.end(); it++){
         kvalobs::kvDbBase *dat=const_cast<T*>(&(*it));

         myList.push_back(dat);
      }

      return insertImpl(myList, tbl, replace);
   }


   template<class T>
      bool insertList(const std::list<T>& li , InsertOption option=INSERT_OR_UPDATE,
                  const miutil::miString &tblName="")
      {
         typename std::list<T>::const_iterator it=li.begin();
         std::list<kvalobs::kvDbBase*> myList;

         if(it==li.end())
            return true;

         std::string tbl;

         if(tblName.empty())
            tbl=it->tableName();
         else
            tbl=tblName;


         for(;it!=li.end(); it++){
            kvalobs::kvDbBase *dat=const_cast<T*>(&(*it));

            myList.push_back(dat);
         }

         return insertImpl(myList, tbl, option);
      }

#endif
    void update(const kvalobs::kvDbBase &elem, const std::string &tblName);

    void exec(const std::string &sql);

    void select(KvalobsDbExtract* extract, const std::string& query);

private:
    void checkConnection();
    void beginTransaction();
    void endTransaction();
    void rollBack();

private:
    Connection_t *mConnection;
    int mBusyTimeout;

};

#endif /* KvalobsDbGate_H */
