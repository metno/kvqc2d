// -*- c++ -*-

#ifndef QC2APP_H
#define QC2APP_H

#include <kvalobs/kvapp.h>
#include <kvskel/kvService.hh>
#include <kvalobs/kvStationInfo.h>
#include <kvdb/dbdrivermgr.h>

#include <boost/thread.hpp>

class Qc2App: public KvApp
{
public:
    Qc2App(int argc, char **argv, 
           const std::string &driver,
           const std::string &connect_,
           const char *options[][2]=0);

    virtual ~Qc2App();

    int run();

    bool sendDataToKvService(const kvalobs::kvStationInfoList &info_, bool &busy);

    void startShutdown()
        { mShouldShutdown = true; }

    bool isShuttingDown();

    /**
     * Creates a new connection to the database. The caller must
     * call releaseDbConnection after use.
     */
    dnmi::db::Connection *getNewDbConnection();
    void                 releaseDbConnection(dnmi::db::Connection *con);

private:
    void initializeDB(const std::string& dbDriver);
    void initializeCORBA();
    void runCORBA();
    void shutdownCORBA();

    CKvalObs::CService::DataReadyInput_ptr lookUpKvService(bool forceNS,
                                                           bool &usedNS);

    Qc2App(); //No implementation

private:
    dnmi::db::DriverManager dbMgr;
    std::string dbConnect;
    std::string dbDriverId;
    bool mShouldShutdown;

    CKvalObs::CManager::CheckedInput_var refServiceCheckedInput;
    CKvalObs::CService::DataReadyInput_var refKvServiceDataReady;

    boost::thread* mCORBAThread;
};

#endif
