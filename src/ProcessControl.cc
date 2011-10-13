#include "ProcessControl.h"

#include "Helpers.h"
#include <kvalobs/kvDataFlag.h>
#include "foreach.h"

/// All flag check chars not set unless set explicitly in a configuration file, the values set
/// are carried around in a map with the index pair corresponding  to the nibble position. This
/// is handled by ReadProgramOptions.cc
/// The condition method searches through a controlinfo and see if any of the char/bits match
/// a flag value set in the config file. 

/// Also can test individual claes, e.g. Aflag[3] (under development ???).

ProcessControl::ProcessControl()
{
}

/// If any of the members of the flag group zflag match the controlinfo TRUE is returned.
bool ProcessControl::condition(const kvalobs::kvControlInfo& controlinfo, const vucflags_t& zflag)
{
    foreach(vucflags_t::value_type v, zflag) {
        if( std::find(v.second.begin(), v.second.end(), controlinfo.cflag(v.first)) != v.second.end() )
            return true;
    }
    return false;
}

bool ProcessControl::true_nibble(const kvalobs::kvControlInfo& controlinfo, const vucflags_t& vlag, int vindex, bool flagbool )
{
    const unsigned char cflag = controlinfo.cflag(vindex);
    const std::vector<unsigned char>& v = vlag.find(vindex)->second; // FIXME check if found
    for (int i=0; i<v.size(); i++) {
        const unsigned char zindex = v[i];
        if( flagbool ) {
            if( cflag == zindex )
                return true;
        } else {
            if( cflag != zindex )
                return true;
        }
    }
    return false;
/// THIS NEEDS CHECKING SINCE EVERYTHING WAS VECTORIZED. 
/// First checks were OK!!! But check for multiple settings too?
}

/// Any values set in the flag group zflag are written into Controlinfo
int ProcessControl::setter( kvalobs::kvControlInfo& controlinfo, const ucflags_t& zflag )
{
    foreach(ucflags_t::value_type f, zflag) {
        const int CC = f.first;
        const char II = Helpers::hexCharToInt( f.second );
        controlinfo.set( CC, II ); 
    }
    return 0; // FIXME why always return 0?
}

/////////////
/// Conditional Setter

int ProcessControl::conditional_setter( kvalobs::kvControlInfo& controlinfo, const vsflags_t& vFlag )
{
    for(int flagIndex = 0; flagIndex < kvalobs::kvControlInfo::size; ++flagIndex) {
        const int flag = controlinfo.flag(flagIndex);
        const std::vector<std::string>& vec = vFlag.find(flagIndex)->second; // FIXME check if found
        foreach(const std::string& s, vec) {
            if( s.size() == 4 && s.substr(1,2) == "->" ) {
                //std::cout << *it << std::endl;
                const int OldFlagValue = Helpers::hexCharToInt( s[0] );
                const int NewFlagValue = Helpers::hexCharToInt( s[3] );
                if( flag == OldFlagValue )
                    controlinfo.set(flagIndex, NewFlagValue);
            }
        }
    }
    return 0; // FIXME why always return 0?
}

// fcc -1 // fclim -1 // fcombi -1 // fcp -1 // fd 7 // fhqc -1 // fmis -1 // fnum -1 // fpos 8 // fpre -1 // fqclevel -1 // fr -1 // fs -1 // fstat -1 // ftime -1 // fw -1 // [0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0]
