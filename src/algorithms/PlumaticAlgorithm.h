// -*- c++ -*-

#ifndef PlumaticAlgorithm_H
#define PlumaticAlgorithm_H 1

#include "Qc2Algorithm.h"
#include "algorithms/DataUpdate.h"
#include <boost/shared_ptr.hpp>

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
        PlumaticUpdate()
            : DataUpdate(), mNotOperationalStart(false), mNotOperationalEnd(false) { }

        PlumaticUpdate(const kvalobs::kvData& data)
            : DataUpdate(data), mNotOperationalStart(false), mNotOperationalEnd(false) { }

        PlumaticUpdate(const kvalobs::kvData& templt, const miutil::miTime& obstime, const miutil::miTime& tbtime,
                       float original, float corrected, const std::string& controlinfo)
            : DataUpdate(templt, obstime, tbtime, original, corrected, controlinfo), mNotOperationalStart(false), mNotOperationalEnd(false) { }

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

    private:
        bool mNotOperationalStart, mNotOperationalEnd;
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

    static int minutesBetween(const miutil::miTime& t0, const miutil::miTime& t1)
        { return miutil::miTime::minDiff(t0, t1); }

    void discardAllNonOperationalTimes(kvUpdateList_t& data);
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

    void checkNeighborStations(int stationid, const kvUpdateList_t& data);
    void compareWithNeighborStations(int stationid, const miutil::miTime& obstime, float sum);

    void storeUpdates(const kvUpdateList_t& data);

    Shower findFirstShower(kvUpdateList_t& data);
    Shower findNextShower(const Shower& s, const kvUpdateList_it& end);
    Shower findShowerForward(const kvUpdateList_it& begin, const kvUpdateList_it& end);

private:
    boost::shared_ptr<RedistributionNeighbors> mNeighbors;

    int pid;
    float mThresholdDry, mThresholdWet;
    FlagSetCU discarded_flags, neighbor_flags;
    FlagChange highsingle_flagchange, highstart_flagchange, interruptedrain_flagchange;
    std::vector<ResolutionStations> mStationlist;
    std::vector<SlidingAlarm> mSlidingAlarms;
    miutil::miTime UT0extended;
};

// ########################################################################

#endif
