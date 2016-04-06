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

#ifndef PlumaticAlgorithm_H
#define PlumaticAlgorithm_H 1

#include "Qc2Algorithm.h"
#include "algorithms/DataUpdate.h"

class RedistributionNeighbors;

// ########################################################################

class PlumaticAlgorithm : public Qc2Algorithm {
public:
    PlumaticAlgorithm();

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

private:
    class PlumaticUpdate : public DataUpdate {
    public:
        PlumaticUpdate();

        PlumaticUpdate(const kvalobs::kvData& data);

        PlumaticUpdate(const kvalobs::kvData& templt, const kvtime::time& obstime, const kvtime::time& tbtime,
                       float original, float corrected, const std::string& controlinfo);

        PlumaticUpdate& setNotOperationalStart()
            { mNotOperationalStart = true; return *this; }

        PlumaticUpdate& setNotOperationalEnd()
            { mNotOperationalEnd = true; return *this; }

        bool isNotOperational() const
            { return mNotOperationalStart || mNotOperationalEnd; }

        bool isNotOperationalStart() const
            { return mNotOperationalStart; }

        bool isNotOperationalEnd() const
            { return mNotOperationalEnd; }

        bool isAggregationFlagged() const
            { return mAggregationFlagged; }

        void setAggregationFlagged(bool af)
            { mAggregationFlagged = af; }

    private:
        bool mNotOperationalStart, mNotOperationalEnd, mAggregationFlagged;
    };

    typedef std::list<kvalobs::kvData> kvDataList_t;
    typedef std::list<PlumaticUpdate> kvUpdateList_t;
    typedef kvUpdateList_t::iterator kvUpdateList_it;
    typedef kvUpdateList_t::const_iterator kvUpdateList_cit;

    struct Shower {
        kvUpdateList_it first, last;
        int duration;
    };
    struct ResolutionStations {
        float mmpv;
        std::vector<int> stationids;
        ResolutionStations(float r)
            : mmpv(r) { }
    };
    struct SlidingAlarm {
        int length;
        float max;
        SlidingAlarm(int l, float m)
            : length(l), max(m) { }
    };

    static int minutesBetween(const kvtime::time& t0, const kvtime::time& t1)
        { return kvtime::minDiff(t0, t1); }

    void discardAllNonOperationalTimes(kvUpdateList_t& data);
    void checkNonOperationalTime(kvUpdateList_t& data, kvUpdateList_it& m1, const kvtime::time& t1,
                                 kvUpdateList_it& m2, const kvtime::time& t2);
    void discardNonOperationalTime(kvUpdateList_t& data, kvUpdateList_it begin, kvUpdateList_it end);

    void checkStation(int stationid, float mmpv);

    void checkSlidingSums(kvUpdateList_t& data);
    void checkSlidingSum(kvUpdateList_t& data, const SlidingAlarm& slal);
    void applyAggregationFlags(kvUpdateList_it start, kvUpdateList_it stop, const SlidingAlarm& slal);

    void checkShowers(kvUpdateList_t& data, float mmpv);
    bool isBadData(const PlumaticUpdate& data);
    bool checkRainInterruption(const Shower& shower, const Shower& previousShower, const float mmpv);
    bool checkHighSingle(const Shower& shower, const float mmpv);
    int  checkHighStartLength(const Shower& shower, const float mmpv);

    void flagRainInterruption(const Shower& shower, const Shower& previousShower, kvUpdateList_t& data);
    void flagHighSingle(const Shower& shower);
    void flagHighStart(const Shower& shower, int length);

    void checkNeighborStations(int stationid, int type, kvUpdateList_t& data);
    int compareWithNeighborStations(int stationid, int type, const kvtime::time& obstime, float sum);
    int countNeighborStations(int stationid, const kvtime::time& obstime,
                              std::vector<int>& dryNeighbors, float& highestDry,
                              std::vector<int>& wetNeighbors, float& lowestWet);

    void storeUpdates(const kvUpdateList_t& data);

    Shower findFirstShower(kvUpdateList_t& data);
    Shower findNextShower(const Shower& s, const kvUpdateList_it& end);
    Shower findShowerForward(const kvUpdateList_it& begin, const kvUpdateList_it& end);

private:
    std::shared_ptr<RedistributionNeighbors> mNeighbors;

    int pid;
    std::vector<int> mTypeIds;
    float mThresholdDry, mThresholdWet;
    int mVippsUnlikelySingle, mVippsUnlikelyStart, mVippsRainInterrupt, mMaxRainInterrupt, mMinRainBeforeAndAfter;
    FlagSetCU discarded_flags, neighbor_flags;
    FlagChange highsingle_flagchange, highstart_flagchange, interruptedrain_flagchange;
    FlagChange fc_no_neighbors, fc_neighbors_ok, fc_neighbors_suspicious;
    std::vector<ResolutionStations> mStationlist;
    std::vector<SlidingAlarm> mSlidingAlarms;
    kvtime::time UT0extended;
};

// ########################################################################

#endif
