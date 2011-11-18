// -*- c++ -*-

#ifndef NeighborsDistance2_H
#define NeighborsDistance2_H 1

#include "RedistributionNeighbors.h"

class NeighborsDistance2 : public RedistributionNeighbors {
public:
    typedef std::map<int, double> stationsWithDistances_t;

    virtual ~NeighborsDistance2() { }

    virtual void configure(const AlgorithmConfig& params);

    virtual stationIDs_t findNeighbors(int aroundID);

    virtual double getWeight(int neighborID);

private:
    float mInterpolationLimit;
    stationsWithDistances_t mDistances;
};

#endif
