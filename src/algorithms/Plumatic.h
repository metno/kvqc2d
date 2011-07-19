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
#include "kvMetadataTable.h"

///The class manages the redistribution of 24 hour precipitation data. 

class Plumatic{

	
private:
  ProcessControl ControlFlag;

public:

  Plumatic( const std::list<kvalobs::kvStation> & slist, ReadProgramOptions params );
  Plumatic(std::list<kvalobs::kvData> & QD, ReadProgramOptions params );
  ~Plumatic(){}

  ReadProgramOptions params;

  miutil::miTime    Start_Time_Interval;
  miutil::miTime    Stop_Time_Interval;

  //std::vector<float> pluvi_data;
  //std::vector<float> pluvi_time;

  std::map<miutil::miTime, float> pluvi_data;

  //std::map<int, std::vector<float> > dst_intp;

  void clear_all();

  int aggregate_window();
  
};

/** @} */
#endif
