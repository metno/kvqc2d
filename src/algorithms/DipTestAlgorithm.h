// -*- c++ -*-

#ifndef DipTestAlgorithm_H
#define DipTestAlgorithm_H 1

#include "Qc2Algorithm.h"

class DipTestAlgorithm : public Qc2Algorithm {
public:
    DipTestAlgorithm(ProcessImpl* p)
        : Qc2Algorithm(p) { }

    virtual void run(const ReadProgramOptions& params);
};

#endif