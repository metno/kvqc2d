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
public:
    RedistributionAlgorithm()
        : Qc2Algorithm("Redistribute") { }

    virtual void run(const ReadProgramOptions& params);

private:
    void findNeighbors(int stationID, NeighborFinder::stationsWithDistances_t& neighbors);
    void getSeriesBefore(const kvalobs::kvData& endpoint, std::list<kvalobs::kvData>& bdata);
    bool checkAndTrimSeriesBefore(std::list<kvalobs::kvData>& bdata, std::list<kvalobs::kvData>& before, const kvalobs::kvData& endpoint);
    void configure(const ReadProgramOptions& params);

private:
    NeighborFinder nf;
    FlagSetCU endpoint_flags, missingpoint_flags, before_flags, neighbor_flags;
    float mInterpolationLimit;
    FlagChange update_flagchange;
    miutil::miTime UT0;
};

#endif
