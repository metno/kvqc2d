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
    void getMissingBefore(const kvalobs::kvData& endpoint, const miutil::miTime& earliest, dataList_t& bdata);
    bool checkAndTrimSeries(dataList_t& bdata);
    bool checkPointBeforeMissing(const dataList_t& accumulation);
    bool getNeighborData(const dataList_t& accumulation, dataList_t& ndata);
    void redistributeDry(dataList_t& accumulation);
    void redistributePrecipitation(dataList_t& accumulation);
    
    miutil::miTime stepTime(const miutil::miTime& time);

private:
    boost::shared_ptr<RedistributionNeighbors> mNeighbors;
    FlagSetCU endpoint_flags, missingpoint_flags, before_flags, neighbor_flags;
    FlagChange update_flagchange;
    miutil::miTime UT0;
    float missing, rejected;
    std::string CFAILED_STRING;
};

#endif
