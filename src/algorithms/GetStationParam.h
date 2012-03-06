// -*- c++ -*-

#ifndef GET_STATION_PARAM_
#define GET_STATION_PARAM_

#include <kvalobs/kvStationParam.h>
#include <list>
#include <map>

class GetStationParam 
{
public:
    GetStationParam(const std::list<kvalobs::kvStationParam>& splist);
    ~GetStationParam() {};
    std::string ValueOf(const std::string& name);

private:
    std::map<std::string,std::string> spMap; 
};


#endif 
