// -*- c++ -*-

#ifndef StatisticalMean_H
#define StatisticalMean_H 1

#include <boost/version.hpp>
#if BOOST_VERSION >= 104000

#include "DBInterface.h"
#include "Qc2Algorithm.h"
#include "algorithms/DataUpdate.h"
#include <boost/shared_ptr.hpp>

class RedistributionNeighbors;

class StatisticalMean : public Qc2Algorithm {
public:
    StatisticalMean();

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

    float getReferenceValue(int station, int dayOfYear, const std::string& key, bool& valid);

private:
    std::list<int> findNeighbors(int stationID);

private:
    boost::shared_ptr<RedistributionNeighbors> mNeighbors;

    float mTolerance;
    int mDays;
    int mDaysRequired;
    int mParamid;
    std::vector<int> mTypeids;
    miutil::miTime mUT0extended;

    FlagSetCU ok_flags;

    typedef std::map<std::string, DBInterface::reference_value_map_t> referenceKeys_t;
    referenceKeys_t mReferenceKeys;
};

#endif /* BOOST_VERSION >= 104000 */

#endif /* StatisticalMean_H */
