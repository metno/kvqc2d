
#include "Qc2Algorithm.h"

#include "CheckedDataHelper.h"
#include "foreach.h"

class RealBroadcaster : public Broadcaster {
public:
    RealBroadcaster(Qc2App& app)
        : checkedDataHelper(app) { }
    virtual void queueChanged(const kvalobs::kvData& d);
    virtual void sendChanges();
private:
    CheckedDataHelper checkedDataHelper;
    kvalobs::kvStationInfoList stList;
};

void RealBroadcaster::queueChanged(const kvalobs::kvData& d)
{
    stList.push_back(kvalobs::kvStationInfo(d.stationID(), d.obstime(), d.typeID()));
}

void RealBroadcaster::sendChanges()
{
    if( stList.empty() )
        return;

    checkedDataHelper.sendDataToService(stList);
    stList.clear();
}

// #######################################################################

Qc2Algorithm::Qc2Algorithm(ProcessImpl* dispatcher)
    : mDispatcher(dispatcher)
    , mBroadcaster(new RealBroadcaster(dispatcher->getApp()))
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
    foreach(const kvalobs::kvStation& s, stations) {
        idList.push_back( s.stationID() );
    }
}

void Qc2Algorithm::fillStationIDList(std::list<int>& idList)
{
    std::list<kvalobs::kvStation> stations;
    fillStationLists(stations, idList);
}
