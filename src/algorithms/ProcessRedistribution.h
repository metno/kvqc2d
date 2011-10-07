// -*- c++ -*-

#ifndef ProcessRedistribution_H
#define ProcessRedistribution_H 1

#include "Qc2Algorithm.h"

class RedistributionAlgorithm : public Qc2Algorithm {
public:
    RedistributionAlgorithm(ProcessImpl* p)
        : Qc2Algorithm(p) { }

    virtual void run(const ReadProgramOptions& params);
};

#endif
