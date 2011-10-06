// -*- c++ -*-

#ifndef _PROCESS_CONTROL_
#define _PROCESS_CONTROL_

#include <kvalobs/kvData.h>
#include <vector>
#include <map>
#include <string>

/// ProcessControl provids methods to set controlinfo flag values and test controlinfo flag values.

class ProcessControl {
public:
    typedef std::map<int, std::vector<unsigned char> > vucflags_t;
    typedef std::map<int, unsigned char> ucflags_t;
    typedef std::map<int, std::vector<std::string> > vsflags_t;

public:
    ProcessControl();

    bool condition( kvalobs::kvControlInfo controlinfo, const vucflags_t& vlag );
    int setter( kvalobs::kvControlInfo &controlinfo, const ucflags_t& vlag );
    int conditional_setter( kvalobs::kvControlInfo &controlinfo, const vsflags_t& vlag );
    bool true_nibble( kvalobs::kvControlInfo controlinfo, const vucflags_t& vlag, int vindex, bool flagbool );
    
protected:
    
private:
    static char hexCharToInt(char n);
};

#endif 
