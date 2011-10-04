// -*- c++ -*-

#ifndef __Qc2Thread_h__
#define __Qc2Thread_h__

#include <string>

class Qc2App;

/// The main Qc2 thread.

class Qc2Work {
private:
    Qc2App& app;
    std::string logpath_;
    
public:
    Qc2Work( Qc2App &app_, const std::string& logpath = "./log" );
    void operator() ();
};

#endif
