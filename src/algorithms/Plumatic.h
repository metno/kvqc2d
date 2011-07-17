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

///The class manages the redistribution of 24 hour precipitation data. 

class Plumatic{

	
private:
  ProcessControl ControlFlag;

public:

  Plumatic( const std::list<kvalobs::kvStation> & slist, ReadProgramOptions params );
  Plumatic(std::list<kvalobs::kvData> & QD, ReadProgramOptions params );
  ~Plumatic(){}

  ReadProgramOptions params;

  std::map<int, std::vector<float> > dst_data;
  std::map<int, std::vector<float> > dst_intp;
  std::map<int, std::vector<float> > dst_corr;
  std::map<int, std::vector<float> > dst_newd;

  std::map<int, std::vector<miutil::miTime> >    dst_time;
  std::map<int, std::vector<miutil::miTime> >    dst_tbtime;

  std::map<int, std::vector<int> > d_sensor;
  std::map<int, std::vector<int> > d_level;
  
  std::map<int, std::vector<int> >    d_typeid;
  std::map<int, std::vector<kvalobs::kvControlInfo> > d_controlinfo;
  std::map<int, std::vector<kvalobs::kvUseInfo> > d_useinfo;
  std::map<int, std::vector<miutil::miString> > d_cfailed;

  void clear_all();
  
};

/** @} */
#endif
