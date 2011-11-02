// -*- c++ -*-

#ifndef SingleLinearAlgorithm_H
#define SingleLinearAlgorithm_H 1

#include "ProcessControl.h"
#include "Qc2Algorithm.h"
#include <kvalobs/kvData.h>

#include <map>
#include <string>
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

    virtual void run(const ReadProgramOptions& params);

private:
    bool isNeighborOk(const ReadProgramOptions& params, const kvalobs::kvData& n);
    float calculateCorrected(const ReadProgramOptions& params, const std::vector<kvalobs::kvData>& series);
    void storeUpdate(const ReadProgramOptions& params, const kvalobs::kvData& middle, const float NewCorrected);

private:
    std::map<int, std::vector<std::string> > setmissing_chflag;
};

#endif
