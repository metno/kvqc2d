
#include "Qc2Algorithm.h"

#include "AlgorithmHelpers.h"
#include "Helpers.h"
#include "foreach.h"
#include <milog/milog.h>

Qc2Algorithm::Qc2Algorithm(const std::string& name)
    : mDatabase(0)
    , mBroadcaster(0)
    , mName(name)
{
}

Qc2Algorithm::~Qc2Algorithm()
{
}

void Qc2Algorithm::fillStationLists(std::list<kvalobs::kvStation>& stations, std::list<int>& idList)
{
    // TODO this list is not updated very often; maybe cache the results somewhere else
    Helpers::GetNorwegianFixedStations(mDatabase, stations);

    idList.clear();
    foreach(const kvalobs::kvStation& s, stations) {
        idList.push_back( s.stationID() );
    }
}

void Qc2Algorithm::fillStationIDList(std::list<int>& idList)
{
    std::list<kvalobs::kvStation> stations;
    fillStationLists(stations, idList);
}

void Qc2Algorithm::updateSingle(const kvalobs::kvData& update)
{
    const std::list<kvalobs::kvData> toUpdate(1, update);
    storeData(toUpdate);
}

void Qc2Algorithm::storeData(const std::list<kvalobs::kvData>& toUpdate, const std::list<kvalobs::kvData>& toInsert)
{
    database()->storeData(toUpdate, toInsert);
    foreach(const kvalobs::kvData& i, toInsert) {
        broadcaster()->queueChanged(i);
        LOGINFO(mName + " NEW ROW: " + Helpers::kvqc2logstring(i) );
    }
    foreach(const kvalobs::kvData& u, toUpdate) {
        broadcaster()->queueChanged(u);
        LOGINFO(mName + ": " + Helpers::kvqc2logstring(u) );
    }
    broadcaster()->sendChanges();
}

void Qc2Algorithm::configure(const ReadProgramOptions& params)
{
    UT0            = params.UT0;
    UT1            = params.UT1;
    CFAILED_STRING = params.CFAILED_STRING;
    missing        = params.missing;
    rejected       = params.rejected;
}
