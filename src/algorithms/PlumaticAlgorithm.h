// -*- c++ -*-

#ifndef PlumaticAlgorithm_H
#define PlumaticAlgorithm_H 1

#include "Qc2Algorithm.h"
#include "algorithms/DataUpdate.h"

// ########################################################################

class PlumaticAlgorithm : public Qc2Algorithm {
public:
    typedef std::list<kvalobs::kvData> kvDataOList_t;
    typedef std::list<DataUpdate> kvDataList_t;
    typedef kvDataList_t::iterator kvDataList_it;

    PlumaticAlgorithm()
        : Qc2Algorithm("Plumatic") { }

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

private:
    class Navigator {
    public:
        Navigator(kvDataList_it begin, kvDataList_it end, const FlagSetCU& df)
            : mBegin(begin), mEnd(end), mLast(--end), discarded_flags(df) { }

        kvDataList_it previousNot0(kvDataList_it it);
        
        kvDataList_it nextNot0(kvDataList_it it);

        kvDataList_it begin()
            { return mBegin; }
        
        kvDataList_it end()
            { return mEnd; }

    private:
        bool is0OrDiscarded(const kvDataList_it& it) const;

    private:
        const kvDataList_it mBegin, mEnd, mLast;
        const FlagSetCU& discarded_flags;
    };

    struct Info {
        Navigator& nav;
        kvDataList_it d, prev, next;
        int dryMinutesBefore, dryMinutesAfter;
        miutil::miTime beforeUT0, afterUT1;
        float mmpv;
        Info(Navigator& n);
    };

    enum CheckResult {
        NO = 0,
        YES = 1,
        DONT_KNOW = 2
    };

    static int minutesBetween(const miutil::miTime& t0, const miutil::miTime& t1)
        { return miutil::miTime::minDiff(t0, t1); }

    void checkStation(int stationid, float mmpv);

    void checkSlidingSums(kvDataList_t& data);
    void checkSlidingSum(kvDataList_t& data, int length, float maxi);

    CheckResult isRainInterruption(const Info& info);
    CheckResult isHighSingle(const Info& info);
    CheckResult isHighStart(const Info& info);

    void flagRainInterruption(Info& info, kvDataList_t& data);
    void flagHighSingle(Info& info);
    void flagHighStart(Info& info);

    void storeUpdates(const kvDataList_t& data);

private:
    int pid;
    FlagSetCU discarded_flags;
    FlagChange highsingle_flagchange, highstart_flagchange, interruptedrain_flagchange, aggregation_flagchange;
    miutil::miString mStationlist, mSlidingAlarms;
    miutil::miTime UT0extended;
};

// ########################################################################

#endif
