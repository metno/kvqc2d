
#include "FlagChange.h"

bool FlagChange::parse(const std::string& flagstring)
{
    if( flagstring.empty() )
        return true;

    std::size_t lastSeparator = 0;
    while( lastSeparator < flagstring.size() ) {
        std::size_t nextSeparator = flagstring.find(";", lastSeparator);
        if( nextSeparator == std::string::npos )
            nextSeparator = flagstring.size();

        FlagMatcher fm;

        std::size_t nextAssign = flagstring.find("->", lastSeparator);
        if( nextAssign == std::string::npos || nextAssign >= nextSeparator ) {
            nextAssign = lastSeparator;
        } else if( nextAssign == lastSeparator ) {
            nextAssign += 2;
        } else {
            if( !fm.parseControlinfo(flagstring.substr(lastSeparator, nextAssign - lastSeparator)) )
                return false;
            nextAssign += 2;
        }

        FlagUpdater fu;
        if( !fu.parse(flagstring.substr(nextAssign, nextSeparator - nextAssign)) )
            return false;

        add(fm, fu);
        lastSeparator = nextSeparator + 1;
    }
    return true;
}

kvalobs::kvControlInfo FlagChange::apply(const kvalobs::kvControlInfo& orig) const
{
    kvalobs::kvControlInfo flag = orig;
    for(unsigned int i=0; i<mMatchers.size(); ++i) {
        if( mMatchers[i].matches(flag) )
            flag = mUpdaters[i].apply(flag);
    }
    return flag;
}
