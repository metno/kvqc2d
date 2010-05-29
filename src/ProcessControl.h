#ifndef _PROCESS_CONTROL_
#define _PROCESS_CONTROL_


#include <kvalobs/kvData.h>
#include <vector>
#include <map>
#include <iostream>
#include <string>

#include "ReadProgramOptions.h"

/// ProcessControl provids methods to set controlinfo flag values and test controlinfo flag values.

class ProcessControl{

public:

   ProcessControl();
   bool condition( kvalobs::kvControlInfo controlinfo, std::map<int, unsigned char> vlag );
   int setter( kvalobs::kvControlInfo &controlinfo, std::map<int, unsigned char> vlag );
   int conditional_setter( kvalobs::kvControlInfo &controlinfo, std::map<int, std::vector<std::string> > vlag );

   bool true_nibble( kvalobs::kvControlInfo controlinfo, std::map<int, unsigned char> vlag, int vindex, bool flagbool );

protected:

private:

   std::map<unsigned char,int> HexToInt;
   std::map<std::string,int> StringToInt;

};


#endif 
