// -*- c++ -*-

#ifndef __Distribute_h__
#define __Distribute_h__

#include <kvalobs/kvDbBase.h>
#include <kvalobs/kvDataFlag.h>
#include <kvalobs/kvStation.h>
#include <kvalobs/kvData.h>
#include <vector>
#include <map>


#include "ReadProgramOptions.h"
#include "ProcessControl.h"
#include "scone.h"

///The class manages the redistribution of 24 hour precipitation data.

class Distribute {

private:
    ProcessControl ControlFlag;

public:
    struct StationData {
        kvalobs::kvData mObservation;
        float mInterpolated;
        float mRedis;
        StationData(const kvalobs::kvData& o, float interpolated, float redis)
            : mObservation(o), mInterpolated(interpolated), mRedis(redis) { }
        StationData();
    };

    void add_element(const StationData& sd);
    void clean_station_entry(int sid);

    // void RedistributeStationData( int & sid , std::list<kvalobs::kvData>& ReturnData );
    void RedistributeStationData(int sid, std::list<kvalobs::kvData>& ReturnData, const ReadProgramOptions& params);

    void clear_all();

private:
    typedef std::map<int, std::vector<StationData> > stationsByID_t;
    stationsByID_t mStationsByID;
};

#endif
