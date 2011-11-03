// -*- c++ -*-

#ifndef RedistributionAlgorithm_H
#define RedistributionAlgorithm_H 1

#include "Qc2Algorithm.h"

class RedistributionAlgorithm2 : public Qc2Algorithm {
public:
    RedistributionAlgorithm2()
        : Qc2Algorithm("Redistribute") { }

    virtual void run(const ReadProgramOptions& params);

private:
    void configure(const ReadProgramOptions& params);

private:
    FlagSetCU endpoint_flags, missingpoint_flags, before_flags, neighbor_flags;
    float mInterpolationLimit;
    FlagChange update_flagchange;
};

#endif
