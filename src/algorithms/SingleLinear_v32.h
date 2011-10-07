// -*- c++ -*-

#ifndef SingleLinear_v32_H
#define SingleLinear_v32_H 1

#include "CheckedDataHelper.h"
#include "ProcessControl.h"
#include "Qc2Algorithm.h"
#include <kvalobs/kvData.h>

#include <map>
#include <string>
#include <vector>

class SingleLinearV32Algorithm : public Qc2Algorithm {
public:
    SingleLinearV32Algorithm(ProcessImpl* p);

    virtual void run(const ReadProgramOptions& params);

private:
    bool isNeighborOk(const ReadProgramOptions& params, const kvalobs::kvData& n);
    float calculateCorrected(const ReadProgramOptions& params, const kvalobs::kvData& before,
                             const kvalobs::kvData& middle, const kvalobs::kvData& after,
                             const int stationID, const miutil::miTime& timeAfter);
    void storeUpdate(const ReadProgramOptions& params, const kvalobs::kvData& middle, const float NewCorrected);

private:
    CheckedDataHelper checkedDataHelper;
    ProcessControl CheckFlags;
    std::map<int, std::vector<std::string> > setmissing_chflag;
};

#endif
