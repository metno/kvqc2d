// -*- c++ -*-

#ifndef DipTestAlgorithm_H
#define DipTestAlgorithm_H 1

#include "Qc2Algorithm.h"

/**
 * See https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:requirements:algorithms:diptest03
 */
class DipTestAlgorithm : public Qc2Algorithm {
public:
    DipTestAlgorithm()
        : Qc2Algorithm("DipTest") { }

    virtual void run(const ReadProgramOptions& params);

private:
    bool fillParameterDeltaMap(const ReadProgramOptions& params, std::map<int, float>& map);
};

#endif
