// -*- c++ -*-

#ifndef SingleLinear_v32_H
#define SingleLinear_v32_H 1

#include "Qc2Algorithm.h"

class SingleLinearV32Algorithm : public Qc2Algorithm {
public:
    SingleLinearV32Algorithm(ProcessImpl* p)
        : Qc2Algorithm(p) { }

    virtual void run(const ReadProgramOptions& params);

};

#endif
