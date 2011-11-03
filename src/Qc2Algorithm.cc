
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

void Qc2Algorithm::updateData(const kvalobs::kvData& toWrite)
{
    database()->insertData(toWrite, true);
    broadcaster()->queueChanged(toWrite);
    broadcaster()->sendChanges();
}

void Qc2Algorithm::updateData(const std::list<kvalobs::kvData>& toWrite)
{
    database()->insertData(toWrite, true);
    foreach(const kvalobs::kvData& w, toWrite) {
        broadcaster()->queueChanged(w);
        LOGINFO(mName + ": " + Helpers::kvqc2logstring(w) );
    }
    broadcaster()->sendChanges();
}
