// -*- c++ -*-

#ifndef QC2THREAD_H
#define QC2THREAD_H 1

class Qc2App;

class Qc2Work {
private:
    Qc2App& app;
    
public:
    Qc2Work( Qc2App &app_ );

    void operator() ()
        { run(); }

    void run();
};

#endif
