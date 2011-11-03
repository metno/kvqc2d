// -*- c++ -*-

#ifndef GapInterpolationAlgorithm_H
#define GapInterpolationAlgorithm_H 1

#include "Qc2Algorithm.h"

class GapInterpolationAlgorithm : public Qc2Algorithm {
public:
    GapInterpolationAlgorithm()
        : Qc2Algorithm("GapInterpolate") { }

    virtual void run(const ReadProgramOptions& params);
};

#endif
