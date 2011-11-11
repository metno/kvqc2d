// -*- c++ -*-

#ifndef PlumaticAlgorithm_H
#define PlumaticAlgorithm_H 1

#include "Qc2Algorithm.h"

class PlumaticAlgorithm : public Qc2Algorithm {
public:
    typedef std::list<kvalobs::kvData> kvDataList_t;
    typedef kvDataList_t::iterator kvDataList_it;

    PlumaticAlgorithm()
        : Qc2Algorithm("Plumatic") { }

    virtual void run(const ReadProgramOptions& params);

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
        Info(Navigator& n);
    };

    bool isRainInterruption(const Info& info);
    bool isHighSingle(const Info& info);
    bool isHighStart(const Info& info);
};

#endif
