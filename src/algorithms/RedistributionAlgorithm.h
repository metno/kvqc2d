// -*- c++ -*-

#ifndef RedistributionAlgorithm_H
#define RedistributionAlgorithm_H 1

#include "Qc2Algorithm.h"

class NeighborFinder {
public:
    typedef std::map<int, double> stationsWithDistances_t;
    typedef std::list<kvalobs::kvStation> stations_t;
    typedef std::map<int, kvalobs::kvStation> stationsByID_t;

    virtual ~NeighborFinder() { }

    bool hasStationList() const
        { return !mStationsByID.empty(); }

    void setStationList(const stations_t& stations);

    virtual void findNeighbors(stationsWithDistances_t& neighbors, int aroundID, float maxdist);

private:
    stationsByID_t mStationsByID;
};

// ########################################################################

class RedistributionAlgorithm : public Qc2Algorithm {
private:
    typedef std::list<kvalobs::kvData> dataList_t;

public:
    RedistributionAlgorithm()
        : Qc2Algorithm("Redistribute") { }

    virtual void run(const ReadProgramOptions& params);

private:
    void findNeighbors(int stationID, NeighborFinder::stationsWithDistances_t& neighbors);
    void getMissingBefore(const kvalobs::kvData& endpoint, dataList_t& bdata);
    bool checkAndTrimSeries(dataList_t& bdata);
    bool checkPointBeforeMissing(const kvalobs::kvData& firstMissing);
    void configure(const ReadProgramOptions& params);
    
    miutil::miTime stepTime(const miutil::miTime& time);

private:
    NeighborFinder nf;
    FlagSetCU endpoint_flags, missingpoint_flags, before_flags, neighbor_flags;
    float mInterpolationLimit;
    FlagChange update_flagchange;
    miutil::miTime UT0;
    float missing, rejected;
};

#endif
