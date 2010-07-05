#include "ProcessControl.h"

#include <kvalobs/kvDataFlag.h>

#include "algorithms/scone.h"

/// All flag check chars not set unless set explicitely in a configuration file, the values set
/// are carried around in a map with the index pair corresponding  to the nibble position. This
/// is handled by ReadProgramOptions.cc
/// The condition method searches through a controlinfo and see if any of the char/bits match
/// a flag value set in the config file. 

/// Also can test individual claes, e.g. Aflag[3] (under development ???).



ProcessControl::
ProcessControl(){
   HexToInt['0']=0;
   HexToInt['1']=1;
   HexToInt['2']=2;
   HexToInt['3']=3;
   HexToInt['4']=4;
   HexToInt['5']=5;
   HexToInt['6']=6;
   HexToInt['7']=7;
   HexToInt['8']=8;
   HexToInt['9']=9;
   HexToInt['A']=10;
   HexToInt['B']=11;
   HexToInt['C']=12;
   HexToInt['D']=13;
   HexToInt['E']=14;
   HexToInt['F']=15;
   StringToInt["0"]=0;
   StringToInt["1"]=1;
   StringToInt["2"]=2;
   StringToInt["3"]=3;
   StringToInt["4"]=4;
   StringToInt["5"]=5;
   StringToInt["6"]=6;
   StringToInt["7"]=7;
   StringToInt["8"]=8;
   StringToInt["9"]=9;
   StringToInt["A"]=10;
   StringToInt["B"]=11;
   StringToInt["C"]=12;
   StringToInt["D"]=13;
   StringToInt["E"]=14;
   StringToInt["F"]=15;
}

/// If any of the members of the flag group zflag match the controlinfo TRUE is returned.
bool 
ProcessControl::
condition(kvalobs::kvControlInfo controlinfo, std::map<int, std::vector<unsigned char> > zflag){

  int filter=0;
  int k;

  for (std::map<int, std::vector<unsigned char> >::const_iterator ik=zflag.begin(); ik != zflag.end(); ++ik) {
       for (k=0;k<(*ik).second.size();k++) {
           if ( (*ik).second[k] == controlinfo.cflag((*ik).first) ) ++filter;
       }
  }

  if (filter) {
      return true;}
  else{    
      return false;}
}

bool
ProcessControl::
true_nibble( kvalobs::kvControlInfo controlinfo, std::map<int, std::vector<unsigned char> > vlag, int vindex, bool flagbool ){

unsigned char zindex;


for (int i=0;i<vlag[vindex].size();i++) {

        zindex=vlag[vindex][i];
  
        if (flagbool) { 
            if (controlinfo.cflag(vindex)==zindex) { 
                return true;}
        } else {
            if (controlinfo.cflag(vindex)!=zindex) { 
                return true;}
       }
   }
return false;
/// THIS NEEDS CHECKING SINCE EVERYTHING WAS VECTORIZED. 
/// First checks were OK!!! But check for multiple settings too?
}

/// Any values set in the flag group zflag are written into Controlinfo
int 
ProcessControl::
setter( kvalobs::kvControlInfo &controlinfo, std::map<int, unsigned char> zflag ){

  int CC, II;

  for (std::map<int, unsigned char>::const_iterator ik=zflag.begin(); ik != zflag.end(); ++ik) {
      CC=(*ik).first; 
      II=HexToInt[ (*ik).second ];  ///Review this !!!
      controlinfo.set( CC, II ); 
  }
   return 0;
}

/////////////
/// Conditional Setter

int 
ProcessControl::
conditional_setter( kvalobs::kvControlInfo &controlinfo, std::map<int, std::vector<std::string> > vFlag ){

  int OldFlagValue, NewFlagValue;

  //std::cout << controlinfo << std::endl;

  for (int inib = 0; inib < 16; ++inib) {
     for (std::vector<std::string>::iterator it = vFlag[inib].begin(); it!=vFlag[inib].end(); ++it) {
       if (it->substr(1,2)=="->" & it->size() == 4) {
             //std::cout << *it << std::endl;
             OldFlagValue=StringToInt[ it->substr(0,1) ];
             NewFlagValue=StringToInt[ it->substr(3,1) ];
             if (controlinfo.flag(inib) == OldFlagValue) controlinfo.set( inib, NewFlagValue ); 
       }
     }
  }

  //std::cout << controlinfo << std::endl;

return 0;
}


// fcc -1 // fclim -1 // fcombi -1 // fcp -1 // fd 7 // fhqc -1 // fmis -1 // fnum -1 // fpos 8 // fpre -1 // fqclevel -1 // fr -1 // fs -1 // fstat -1 // ftime -1 // fw -1 // [0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0]


