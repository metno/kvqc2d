// -*- c++ -*-

#ifndef PlumaticAlgorithm_H
#define PlumaticAlgorithm_H 1

#include "Qc2Algorithm.h"
#include "algorithms/DataUpdate.h"

// ########################################################################

class PlumaticAlgorithm : public Qc2Algorithm {
public:
    PlumaticAlgorithm()
        : Qc2Algorithm("Plumatic") { }

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

private:
    class PlumaticUpdate : public DataUpdate {
    public:
        PlumaticUpdate()
            : DataUpdate(), mNotOperational(false) { }

        PlumaticUpdate(const kvalobs::kvData& data)
            : DataUpdate(data), mNotOperational(false) { }

        PlumaticUpdate(const kvalobs::kvData& templt, const miutil::miTime& obstime, const miutil::miTime& tbtime,
                       float original, float corrected, const std::string& controlinfo)
            : DataUpdate(templt, obstime, tbtime, original, corrected, controlinfo), mNotOperational(false) { }

        PlumaticUpdate& setNotOperational()
            { mNotOperational = true; return *this; }

        bool isNotOperational() const
            { return mNotOperational; }

    private:
        bool mNotOperational;
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

    void checkShowers(kvUpdateList_t& data, float mmpv);
    bool isBadData(const PlumaticUpdate& data);
    bool checkRainInterruption(const Shower& shower, const Shower& previousShower, const float mmpv);
    bool checkHighSingle(const Shower& shower, const float mmpv);
    int  checkHighStartLength(const Shower& shower, const float mmpv);

    void flagRainInterruption(const Shower& shower, const Shower& previousShower, kvUpdateList_t& data);
    void flagHighSingle(const Shower& shower);
    void flagHighStart(const Shower& shower, int length);

    void storeUpdates(const kvUpdateList_t& data);

    Shower findFirstShower(kvUpdateList_t& data);
    Shower findNextShower(const Shower& s, const kvUpdateList_it& end);
    Shower findShowerForward(const kvUpdateList_it& begin, const kvUpdateList_it& end);

private:
    int pid;
    int mWarningDaysNonOperational;
    FlagSetCU discarded_flags;
    FlagChange highsingle_flagchange, highstart_flagchange, interruptedrain_flagchange, aggregation_flagchange;
    std::vector<ResolutionStations> mStationlist;
    std::vector<SlidingAlarm> mSlidingAlarms;
    miutil::miTime UT0extended;
};

// ########################################################################

#endif
