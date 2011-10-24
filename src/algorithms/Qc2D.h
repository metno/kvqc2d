// -*- c++ -*-

#ifndef __Qc2D_h__
#define __Qc2D_h__

#include <kvalobs/kvDbBase.h>
#include <kvalobs/kvDataFlag.h>
#include <kvalobs/kvStation.h>
#include <kvalobs/kvData.h>
#include <vector>
#include <list>
#include <map>

#include "ReadProgramOptions.h"
#include "ProcessControl.h"

/// The Qc2Data object is a custom container for handling data subject to Qc2 algorithms.
/// It is designed to hold data from the whole network and include the geographic
/// co-ordinates and altitude of each point. The geo-statistical algorithms which make up
/// Qc2 space controls require this information to be managed together.
/// Each structure includes a full kvalobs::kvData record
/// with the associated station location (height, latitude, longitude).

class Qc2D{

private:
    struct StationData {
        kvalobs::kvData mObservation;
        float mLon;
        float mLat;
        float mAltitude;

        float mInterpolated;
        float mConfidence; // (e.g. standard deviation from the interpolation)

        StationData(const kvalobs::kvData& o, const kvalobs::kvStation& station);
        StationData();
    };

    typedef std::map<int, StationData> dataByStationID_t;
    dataByStationID_t mDataByStationID;

public:
    ~Qc2D(){};


    Qc2D(const std::list<kvalobs::kvData>& QD, const std::list<kvalobs::kvStation>& SL, const ReadProgramOptions& params, bool generateMissing);

    void clean();

    friend std::ostream& operator<<(std::ostream& stm, const Qc2D &Q);

    void Qc2_interp();

    void distributor(std::list<kvalobs::kvData>& ReturnData,int ClearFlag);  // UMPH !!! ClearFlag

private:
#if 0
    void calculate_intp_all(unsigned int index);
    void calculate_intp_temp(unsigned int index);
#endif
    void idw_intp_limit(StationData& sInterpol);
#if 0
    void intp_delaunay(unsigned int index);
    void intp_dummy(unsigned int index);
    void intp_temp(unsigned int index);
    void calculate_intp_wet_dry(unsigned int index);
    void calculate_intp_h(unsigned int index);  // includes modification due to change in height (h).
    void calculate_intp_sl(unsigned int index, std::list<int> BestStations); // perform an interpolation based on a list of allowed stations (sl).
    void calculate_trintp_sl(unsigned int index, std::list<int> BestStations);  // strict triangle interpolation

    int SampleSemiVariogram();
    int SpaceCheck();


    int write_cdf(const std::list<kvalobs::kvStation> & slist); // write the data record to a CDF file

    void filter(std::vector<float>& fdata, float Min, float Max, float IfMod, float Mod);
#endif

private:
    ProcessControl ControlFlag;
    const ReadProgramOptions&                 params;

    Qc2D();
    Qc2D& operator=(const Qc2D&);
};


/** @} */
#endif
