// -*- c++ -*-

#ifndef StatisticalMean_H
#define StatisticalMean_H 1

#include "Qc2Algorithm.h"
#include "algorithms/DataUpdate.h"
#include <boost/shared_ptr.hpp>

class RedistributionNeighbors;

class StatisticalMean : public Qc2Algorithm {
public:
    StatisticalMean();

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

private:
    std::list<int> findNeighbors(int stationID);

    float getStatisticalMean(int stationid, int day);

private:
    boost::shared_ptr<RedistributionNeighbors> mNeighbors;

    float mTolerance;
    int mDays;
    int mDaysRequired;
    int mParamid;
    std::vector<int> mTypeids;
    miutil::miTime mUT0extended;

    FlagSetCU ok_flags;
};

#endif
