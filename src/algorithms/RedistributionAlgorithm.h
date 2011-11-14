// -*- c++ -*-

#ifndef RedistributionAlgorithm_H
#define RedistributionAlgorithm_H 1

#include "Qc2Algorithm.h"
#include <boost/shared_ptr.hpp>

class RedistributionNeighbors;

class RedistributionAlgorithm : public Qc2Algorithm {
private:
    typedef std::list<kvalobs::kvData> dataList_t;

public:
    RedistributionAlgorithm();

    virtual void run(const ReadProgramOptions& params);

private:
    void configure(const ReadProgramOptions& params);
    std::list<int> findNeighbors(int stationID);

    bool findMissing(const kvalobs::kvData& endpoint, const kvalobs::kvData& beforeMissing, const miutil::miTime& fakeTableTime, dataList_t& mdata);
    bool findPointBeforeMissing(const kvalobs::kvData& endpoint, const miutil::miTime& earliest, kvalobs::kvData& latestBefore);

    bool getNeighborData(const dataList_t& accumulation, dataList_t& ndata);
    void redistributeDry(const dataList_t& accumulation, dataList_t& toWrite);
    void redistributePrecipitation(dataList_t& accumulation, dataList_t& toWrite);
    void updateOrInsertData(const dataList_t& toStore, const miutil::miTime& fakeTableTime);
    
    miutil::miTime stepTime(const miutil::miTime& time);

private:
    boost::shared_ptr<RedistributionNeighbors> mNeighbors;
    FlagSetCU endpoint_flags, missingpoint_flags, before_flags, neighbor_flags;
    FlagChange update_flagchange;
    miutil::miTime UT0;
    float missing, rejected;
    std::string CFAILED_STRING;
    std::vector<int> pids;
    std::vector<int> tids;
};

#endif
