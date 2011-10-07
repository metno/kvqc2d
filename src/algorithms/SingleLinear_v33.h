// -*- c++ -*-

#ifndef SingleLinear_v33_H
#define SingleLinear_v33_H 1

#include "Qc2Algorithm.h"

class SingleLinearV33Algorithm : public Qc2Algorithm {
public:
    SingleLinearV33Algorithm(ProcessImpl* p)
        : Qc2Algorithm(p) { }

    virtual void run(const ReadProgramOptions& params);
};

#endif
