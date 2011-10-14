// -*- c++ -*-

#ifndef ProcessRedistribution_H
#define ProcessRedistribution_H 1

#include "Qc2Algorithm.h"

class RedistributionAlgorithm : public Qc2Algorithm {
public:
    RedistributionAlgorithm()
        : Qc2Algorithm() { }

    virtual void run(const ReadProgramOptions& params);
};

#endif
