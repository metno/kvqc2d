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
    bool  fillParameterDeltaMap(const ReadProgramOptions& params, std::map<int, float>& map);
    float fetchDelta(const miutil::miTime& time, int pid);
    void  configure(const ReadProgramOptions& params);
    void  checkDipAndInterpolate(const kvalobs::kvData& candidate, float delta);
    bool  tryAkima(const kvalobs::kvData& candidate, float& interpolated);
    void  writeChanges(const kvalobs::kvData& dip, const kvalobs::kvData& after, const float interpolated, bool haveAkima);

private:
    std::map<int, float> PidValMap;
    std::list<int> StationIds;
    FlagSetCU akima_flags, candidate_flags, linear_before_flags, linear_after_flags;
    FlagChange dip_flagchange, afterdip_flagchange;
    std::string CFAILED_STRING;
    float missing;
};

#endif
