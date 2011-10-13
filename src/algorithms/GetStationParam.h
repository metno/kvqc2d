
#ifndef GET_STATION_PARAM_
#define GET_STATION_PARAM_

#include <kvalobs/kvStationParam.h>
#include <puTools/miString.h>
#include <list>
#include <map>

class GetStationParam 
{
public:
    GetStationParam(const std::list<kvalobs::kvStationParam>& splist);
    ~GetStationParam() {};
	miutil::miString ValueOf(const miutil::miString& name);

private:
    std::map<miutil::miString,miutil::miString> spMap; 
};


#endif 
