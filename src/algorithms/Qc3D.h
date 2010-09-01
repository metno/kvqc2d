#ifndef __Qc3D_h__
#define __Qc3D_h__

#include <kvalobs/kvDbBase.h>
#include <kvalobs/kvDataFlag.h>
#include <kvalobs/kvStation.h>
#include <kvalobs/kvData.h>
#include <vector>
#include <list>
#include <map>

#include "ReadProgramOptions.h"
#include "ProcessControl.h"

/// The Qc3Data object is a custom container for handling data subject to Qc2 algorithms. 
/// It is designed to hold data from the whole network and include the geographic 
/// co-ordinates and altitude of each point. The geo-statistical algorithms which make up 
/// Qc2 space controls require this information to be managed together.
/// Each structure includes a full kvalobs::kvData record 
/// with the associated station location (height, latitude, longitude). 

class Qc3D{
	
private:

public:
  std::vector<int>                    stid_;
  std::vector<miutil::miTime>         obstime_;
  std::vector<float>                  original_;
  std::vector<int>                    paramid_;
  std::vector<miutil::miTime>         tbtime_;
  std::vector<int>                    typeid_;
  std::vector<int>                    sensor_;
  std::vector<int>                    level_;
  std::vector<float>                  corrected_;
  std::vector<kvalobs::kvControlInfo> controlinfo_;
  std::vector<kvalobs::kvUseInfo>     useinfo_;
  std::vector<miutil::miString>       cfailed_;

  std::vector<float>                  intp_;
  std::vector<float>                  redis_;
  std::vector<float>                  lat_;
  std::vector<float>                  lon_;
  std::vector<float>                  ht_;
  std::vector<float>                  CP_;      // A confidence paramter (e.g. standard deviation from the interpolation)

  ReadProgramOptions                  params;

  std::map<int, int>                  stindex; //use this to lookup index based on station id.
  void istindex(int stid)             {stindex[ stid ] = stid_.size()-1;}

                                      //maps the station id to the index of the vectors
                                      //as they are populated ... does this work for all 
                                      //compilers?? i.e. size can never be 0 because an
                                      //element is always created in the statement.


  std::vector<float> intp()                         {return intp_;}

  void istid(int stid)                                  {stid_.push_back(stid);}
  void iobstime(miutil::miTime obstime)                    {obstime_.push_back(obstime);}
  void ioriginal(float original_value)                      {original_.push_back(original_value);}
  void iparamid(int parameter)                              {paramid_.push_back(parameter);}
  void itbtime(miutil::miTime tbtime)                   {tbtime_.push_back(tbtime);}
  void itypeid(int type_id)                             {typeid_.push_back(type_id);}
  void isensor(int sensor)                              {sensor_.push_back(sensor);}
  void ilevel(int level)                               {level_.push_back(level);}
  void icorrected(float corrected_value)                     {corrected_.push_back(corrected_value);}
  void icontrolinfo(kvalobs::kvControlInfo controlinfo) {controlinfo_.push_back(controlinfo); }
  void iuseinfo(kvalobs::kvUseInfo useinfo)             {useinfo_.push_back(useinfo); }
  void icfailed(miutil::miString  cfailed)              {cfailed_.push_back(cfailed); }

  void iintp(float interp_value)                        {intp_.push_back(interp_value);}
  void iredis(float redistributed_value)                {redis_.push_back(redistributed_value);}
  void ilat(float latitude)                             {lat_.push_back(latitude);}
  void ilon(float longitude)                            {lon_.push_back(longitude);}
  void iht(float height)                                {ht_.push_back(height);}

  void icp(float CP)                                    {CP_.push_back(CP);}

  int stationID() const { return stid_[0]; }

  Qc3D(){clean();}
  ~Qc3D(){};

  Qc3D(std::list<kvalobs::kvData>& QD, std::list<kvalobs::kvStation>& SL, ReadProgramOptions params);

  void clean();
 
  friend std::ostream& operator<<(std::ostream& stm, const Qc3D &Q);

private:
  ProcessControl ControlFlag;
};


/** @} */
#endif