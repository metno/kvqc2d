// -*- c++ -*-

#ifndef RedistributionAlgorithm_H
#define RedistributionAlgorithm_H 1

#include "Qc2Algorithm.h"
#include "algorithms/DataUpdate.h"
#include <boost/shared_ptr.hpp>

class RedistributionNeighbors;

class RedistributionAlgorithm : public Qc2Algorithm {
private:
    class RedisUpdate : public DataUpdate {
    public:
        RedisUpdate()
            : DataUpdate(), mHasNeigboursWithPrecipitation(false) { }

        RedisUpdate(const kvalobs::kvData& data)
            : DataUpdate(data), mHasNeigboursWithPrecipitation(false) { }

        RedisUpdate(const kvalobs::kvData& templt, const miutil::miTime& obstime, const miutil::miTime& tbtime,
                    float corrected, const std::string& controlinfo)
            : DataUpdate(templt, obstime, tbtime, corrected, controlinfo), mHasNeigboursWithPrecipitation(false) { }

        RedisUpdate& setHasNeighborsWithPrecipitation()
            { mHasNeigboursWithPrecipitation = true; return *this; }

        bool hasNeighborsWithPrecipitation() const
            { return mHasNeigboursWithPrecipitation; }

        private:
        bool mHasNeigboursWithPrecipitation;
    };

    typedef std::list<kvalobs::kvData> dataList_t;
    typedef dataList_t::iterator dataList_it;
    typedef std::list<RedisUpdate> updateList_t;
    typedef updateList_t::iterator updateList_it;

public:
    RedistributionAlgorithm();

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

private:
    std::list<int> findNeighbors(int stationID);

    bool findMissing(const kvalobs::kvData& endpoint, const kvalobs::kvData& beforeMissing, updateList_t& accumulation);
    bool findPointBeforeMissing(const kvalobs::kvData& endpoint, const miutil::miTime& earliest, kvalobs::kvData& latestBefore);

    bool getNeighborData(const updateList_t& accumulation, dataList_t& ndata);
    void redistributeDry(updateList_t& accumulation);
    bool redistributePrecipitation(updateList_t& accumulation);
    void updateOrInsertData(const updateList_t& toStore);
    
    miutil::miTime stepTime(const miutil::miTime& time);

private:
    boost::shared_ptr<RedistributionNeighbors> mNeighbors;
    FlagSetCU endpoint_flags, missingpoint_flags, before_flags, neighbor_flags;
    FlagChange update_flagchange;
    std::vector<int> pids;
    std::vector<int> tids;
};

#endif
