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
            : DataUpdate(), mHasNeigboursWithPrecipitation(false), mHasAllNeighborsBoneDry(false) { }

        RedisUpdate(const kvalobs::kvData& data)
            : DataUpdate(data), mHasNeigboursWithPrecipitation(false), mHasAllNeighborsBoneDry(false) { }

        RedisUpdate(const kvalobs::kvData& templt, const miutil::miTime& obstime, const miutil::miTime& tbtime,
                    float original, float corrected, const std::string& controlinfo)
            : DataUpdate(templt, obstime, tbtime, original, corrected, controlinfo),
              mHasNeigboursWithPrecipitation(false), mHasAllNeighborsBoneDry(false) { }

        RedisUpdate& setHasNeighborsWithPrecipitation()
            { mHasNeigboursWithPrecipitation = true; return *this; }

        bool hasNeighborsWithPrecipitation() const
            { return mHasNeigboursWithPrecipitation; }

        RedisUpdate& setHasAllNeighborsBoneDry(bool anbd)
            { mHasAllNeighborsBoneDry = anbd; return *this; }

        bool hasAllNeighborsBoneDry() const
            { return mHasAllNeighborsBoneDry; }

    private:
        bool mHasNeigboursWithPrecipitation;
        bool mHasAllNeighborsBoneDry;
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

    bool findMissing(const kvalobs::kvData& endpoint, const miutil::miTime& earliest, updateList_t& accumulation);

    bool getNeighborData(const updateList_t& accumulation, dataList_t& ndata);
    void redistributeBoneDry(updateList_t& accumulation);
    bool redistributePrecipitation(updateList_t& accumulation);
    void updateOrInsertData(const updateList_t& toStore);
    
    miutil::miTime stepTime(const miutil::miTime& time);

private:
    boost::shared_ptr<RedistributionNeighbors> mNeighbors;
    FlagSetCU endpoint_flags, missingpoint_flags, neighbor_flags, warn_and_stop_flags;
    FlagChange update_flagchange;
    std::vector<int> pids;
    std::vector<int> tids;
    int mMeasurementHour, mMinNeighbors, mDaysBeforeNoNeighborWarning;
};

#endif
