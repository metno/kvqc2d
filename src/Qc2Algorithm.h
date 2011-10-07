// -*- c++ -*-

#ifndef Qc2Algorithm_H
#define Qc2Algorithm_H

class ProcessImpl;
#include "ReadProgramOptions.h"

/// Interface implemented by the different QC2 algorithms.

class Qc2Algorithm  
{
public:
    Qc2Algorithm(ProcessImpl* dispatcher);
    virtual ~Qc2Algorithm();

    virtual void run(const ReadProgramOptions& params) = 0;

    ProcessImpl* dispatcher() const
        { return mDispatcher; }

private:
    ProcessImpl* mDispatcher;
};

#endif
