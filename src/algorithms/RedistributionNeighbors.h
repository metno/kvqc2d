// -*- c++ -*-

#ifndef RedistributionNeighbors_H
#define RedistributionNeighbors_H 1

#include <kvalobs/kvStation.h>
#include <list>
class ReadProgramOptions;

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

    virtual void configure(const ReadProgramOptions& params) = 0;

    virtual stationIDs_t findNeighbors(int aroundID) = 0;

    virtual double getWeight(int neighborID) = 0;

protected:
    stationsByID_t mStationsByID;
};

#endif
