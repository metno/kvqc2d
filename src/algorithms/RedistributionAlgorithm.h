// -*- c++ -*-

#ifndef RedistributionAlgorithm_H
#define RedistributionAlgorithm_H 1

#include "Qc2Algorithm.h"

class RedistributionAlgorithm2 : public Qc2Algorithm {
public:
    RedistributionAlgorithm2()
        : Qc2Algorithm() { }

    virtual void run(const ReadProgramOptions& params);
};

#endif
