// -*- c++ -*-

#ifndef PlumaticAlgorithm_H
#define PlumaticAlgorithm_H 1

#include "Qc2Algorithm.h"

class PlumaticAlgorithm : public Qc2Algorithm {
public:
    PlumaticAlgorithm()
        : Qc2Algorithm("Plumatic") { }

    virtual void run(const ReadProgramOptions& params);
};

#endif
