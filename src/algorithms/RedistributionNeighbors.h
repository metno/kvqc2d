// -*- c++ -*-

#ifndef RedistributionNeighbors_H
#define RedistributionNeighbors_H 1

#include <kvalobs/kvStation.h>
#include <list>
#include <map>

class AlgorithmConfig;

class RedistributionNeighbors {
public:
    typedef std::list<int> stationIDs_t;
    typedef std::list<kvalobs::kvStation> stations_t;

protected:
    typedef std::map<int, kvalobs::kvStation> stationsByID_t;

public:
    virtual ~RedistributionNeighbors() { }

    bool hasStationList() const
        { return !mStationsByID.empty(); }

    void setStationList(const stations_t& stations);

    virtual void configure(const AlgorithmConfig& params) = 0;

    virtual stationIDs_t findNeighbors(int aroundID) = 0;

    /** Return weight for a neighbor. If weight < 1, the neighbor is
     * not really good -- weights need to be scaled accordingly.
     */
    virtual double getWeight(int neighborID) = 0;

protected:
    stationsByID_t mStationsByID;
};

#endif
