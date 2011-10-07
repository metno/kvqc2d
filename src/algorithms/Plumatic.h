// -*- c++ -*-

#ifndef Plumatic_H
#define Plumatic_H

#include "ReadProgramOptions.h"
#include "ProcessControl.h"

#include <kvalobs/kvStation.h>
#include <kvalobs/kvData.h>
#include <puTools/miTime.h>

#include <list>
#include <map>

///The class manages the redistribution of 24 hour precipitation data. 

class Plumatic {

private:
    ProcessControl ControlFlag;

public:

    Plumatic( const std::list<kvalobs::kvStation> & slist, const ReadProgramOptions& params );
    Plumatic(std::list<kvalobs::kvData> & QD, const ReadProgramOptions& params );
    ~Plumatic(){}

    void clear_all();

    int aggregate_window(const ReadProgramOptions& params, std::list<miutil::miTime> & TimeList);
  
private:
    const ReadProgramOptions& params;

    miutil::miTime    Start_Time_Interval;
    miutil::miTime    Stop_Time_Interval;

    //std::vector<float> pluvi_data;
    //std::vector<float> pluvi_time;

    std::map<miutil::miTime, float> pluvi_data;

    //std::map<int, std::vector<float> > dst_intp;
};

/** @} */
#endif /* Plumatic_H */
