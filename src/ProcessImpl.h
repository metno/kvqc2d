#ifndef __Qc2Process_h__
#define __Qc2Process_h__


#include <kvalobs/kvStationInfo.h>
#include <kvalobs/kvStation.h>
#include <kvalobs/kvData.h>
#include <string>
#include <stack>

#include "ReadProgramOptions.h"
#include "scone.h"

class Qc2App;

///Handles the interface to different processing algorithms. 

class ProcessImpl  
{
    Qc2App & app;
    std::string logpath_; 
    dnmi::db::Connection & con;

  public:

    ProcessImpl( Qc2App &app_, dnmi::db::Connection & con_ );
    void GetStationList(std::list<kvalobs::kvStation>& StationList);
    void GetStationList(std::list<kvalobs::kvStation>& StationList, miutil::miTime ProcessTime);
    int select(ReadProgramOptions params);
    int Redistribute(ReadProgramOptions params);
    //int locust_alg(ReadProgramOptions params);

    //int Process4D(int pid, int tid, miutil::miTime stime, miutil::miTime etime, std::string CIF);
    int SingleLinear_v32(ReadProgramOptions params);                                                           
    int SingleLinear_v33(ReadProgramOptions params);                                                           
    int GapInterpolate(ReadProgramOptions params);                                                           
    int DipTest(ReadProgramOptions params);                                                           

    //utilities
    std::string kvqc2logstring(kvalobs::kvData kd);
};



#endif
