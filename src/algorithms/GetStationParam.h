/****************************************************************
**
** Definition of a simple Get Station Paramters class
**
****************************************************************/

#ifndef _GET_STATION_PARAM_
#define _GET_STATION_PARAM_

#include <vector>
#include <list>
#include <map>
#include <kvalobs/kvStationParam.h>

#include <puTools/miTime.h>
#include <puTools/miString.h>


class GetStationParam 
{
public:

    std::map<miutil::miString,miutil::miString> spMap; 

    GetStationParam(std::list<kvalobs::kvStationParam> splist);
    ~GetStationParam(){};
	miutil::miString ValueOf(miutil::miString name);

protected:

private:

};


#endif 
