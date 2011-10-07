
#include "Qc2Algorithm.h"

#include <boost/foreach.hpp>

Qc2Algorithm::Qc2Algorithm(ProcessImpl* dispatcher)
    : mDispatcher(dispatcher)
{
}

Qc2Algorithm::~Qc2Algorithm()
{
}

void Qc2Algorithm::fillStationLists(std::list<kvalobs::kvStation> stations, std::list<int>& idList)
{
    // TODO this list is not updated very often; maybe cache the results somewhere else
    mDispatcher->GetStationList(stations);

    idList.clear();
    BOOST_FOREACH(const kvalobs::kvStation& s, stations) {
        idList.push_back( s.stationID() );
    }
}

void Qc2Algorithm::fillStationIDList(std::list<int>& idList)
{
    std::list<kvalobs::kvStation> stations;
    fillStationLists(stations, idList);
}
