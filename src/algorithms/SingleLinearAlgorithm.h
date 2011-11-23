// -*- c++ -*-

#ifndef SingleLinearAlgorithm_H
#define SingleLinearAlgorithm_H 1

#include "FlagChange.h"
#include "FlagPatterns.h"
#include "Qc2Algorithm.h"

#include <vector>

/**
 * Specification:
 * https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:requirements
 * section "Simple Interpolations" links to specification article
 * https://kvalobs.wiki.met.no/lib/exe/fetch.php?media=kvoss:system:qc2:qc2-d2_flagg_08_2010_v32.pdf
 *
 * Also see https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:test:algorithms:singlemissingpoint2
 */
class SingleLinearAlgorithm : public Qc2Algorithm {
public:
    SingleLinearAlgorithm();

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

private:
    bool isNeighborOk(const kvalobs::kvData& n);
    float calculateCorrected(const kvalobs::kvData& before, const kvalobs::kvData& middle, const kvalobs::kvData& after);
    void writeChanges(const kvalobs::kvData& middle, const float NewCorrected);

private:
    FlagSetCU missing_flags, neighbor_flags;
    FlagChange update_flagchange, missing_flagchange;
    std::vector<int> pids;
};

#endif
