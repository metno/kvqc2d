// -*- c++ -*-

#ifndef PlumaticAlgorithm_H
#define PlumaticAlgorithm_H 1

#include "Qc2Algorithm.h"
#include <kvalobs/kvDataFlag.h>
#include <set>

// ########################################################################

class DataUpdate {
public:
    DataUpdate();

    DataUpdate(const kvalobs::kvData& data);

    DataUpdate(const kvalobs::kvData& templt, const miutil::miTime& obstime, const miutil::miTime& tbtime,
               float corrected, const std::string& controlinfo);

    bool isModified() const;

    bool isNew() const
        { return mNew; }

    kvalobs::kvData& data()
        { return mData; }

    const kvalobs::kvData& data() const
        { return mData; }

    float original() const
        { return mData.original(); }

    miutil::miTime obstime() const
        { return mData.obstime(); }

    kvalobs::kvControlInfo controlinfo() const
        { return mData.controlinfo(); }

    DataUpdate& corrected(float c)
        { mData.corrected(c); return *this; }

    DataUpdate& controlinfo(const kvalobs::kvControlInfo& ci);

    DataUpdate& cfailed(const std::string& cf, const std::string& extra="");

    bool operator<(const DataUpdate& other) const
        { return obstime() < other.obstime(); }

private:
    kvalobs::kvData mData;
    bool mNew;
    kvalobs::kvControlInfo mOrigControl;
    float mOrigCorrected;
    std::string mOrigCfailed;
    miutil::miString mStationlist;
};

std::ostream& operator<<(std::ostream& out, const DataUpdate& du);

// ########################################################################

class PlumaticAlgorithm : public Qc2Algorithm {
public:
    typedef std::list<kvalobs::kvData> kvDataOList_t;
    typedef std::list<DataUpdate> kvDataList_t;
    typedef kvDataList_t::iterator kvDataList_it;

    PlumaticAlgorithm()
        : Qc2Algorithm("Plumatic") { }

    virtual void run(const ReadProgramOptions& params);
    virtual void configure(const ReadProgramOptions& params);

private:
    class Navigator {
    public:
        Navigator(kvDataList_it begin, kvDataList_it end)
            : mBegin(begin), mEnd(end), mLast(--end) { }

        kvDataList_it previousNot0(kvDataList_it it);
        
        kvDataList_it nextNot0(kvDataList_it it);
        
        kvDataList_it begin()
            { return mBegin; }
        
        kvDataList_it end()
            { return mEnd; }

    private:
        const kvDataList_it mBegin, mEnd, mLast;
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

    CheckResult isRainInterruption(const Info& info);
    CheckResult isHighSingle(const Info& info);
    CheckResult isHighStart(const Info& info);

    void flagRainInterruption(Info& info, kvDataList_t& data);
    void flagHighSingle(Info& info);
    void flagHighStart(Info& info);

    void storeUpdates(const kvDataList_t& data);

private:
    int pid;
    FlagChange highsingle_flagchange, highstart_flagchange, interruptedrain_flagchange;
    std::string CFAILED_STRING;
    miutil::miString mStationlist;
    miutil::miTime UT0, UT0extended, UT1;
};

// ########################################################################

#endif
