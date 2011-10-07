// -*- c++ -*-

#ifndef __Qc2Process_h__
#define __Qc2Process_h__

#include <kvalobs/kvStation.h>
#include <string>

class ReadProgramOptions;
class Qc2App;
class Qc2Algorithm;

///Handles the interface to different processing algorithms. 

class ProcessImpl  
{
    Qc2App & app;
    std::string logpath_; 
    dnmi::db::Connection & con;

    typedef std::map<std::string, Qc2Algorithm*> algorithms_t;
    algorithms_t mAlgorithms;

public:
    ProcessImpl( Qc2App &app_, dnmi::db::Connection & con_ );
    ~ProcessImpl();

    void GetStationList(std::list<kvalobs::kvStation>& StationList);
    void GetStationList(std::list<kvalobs::kvStation>& StationList, miutil::miTime ProcessTime);

    int select(const ReadProgramOptions& params);

    Qc2App& getApp()
        { return app; }

    dnmi::db::Connection& getConnection()
        { return con; }
};

#endif
