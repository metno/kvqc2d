
#include "FlagSet.h"

#include <sstream>

bool FlagSet::matches(const kvalobs::kvDataFlag& flags) const
{
    if( mError )
        return false;
    if( mMatchers.empty() )
        return mDefaultIfEmpty;
    for(unsigned int i=0; i<mMatchers.size(); ++i)
        if( mMatchers[i].matches(flags) )
            return true;
    return false;
}

bool FlagSet::parse(const std::string& flagstring)
{
    mMatchers.clear();
    mError = false;

    if( flagstring.empty() )
        return true;

    std::istringstream iflag(flagstring);
    FlagMatcher fm;
    int f = 0;
    while(true) {
        char fc;
        iflag >> fc;
        if( !iflag && f != 0 ) {
            mError = true;
            return false;
        }
        if( fc == '_' || fc == '.' ) {
            // no constraint on flag f
        } else if( fc == '[' || fc == ')' ) {
            // multiple permitted / forbidden flags
            while(true) {
                char fcc;
                iflag >> fcc;
                if( !iflag ) {
                    mError = true;
                    return false;
                }
                if( (fc == ')' && fcc == '(') || (fc=='[' && fcc==']') ) {
                    // end of inclusion/exclusion
                    break;
                }
                if( (fcc>='0' && fcc<='9') || (fcc>='A' && fcc<='F') ) {
                    int fv = (fcc>='0' && fcc<='9') ? (fcc - '0') : (fcc - 'A' + 10);
                    if( fc == '[' ) {
                        fm.permit(f, fv);
                    } else {
                        fm.forbid(f, fv);
                    }
                } else {
                    mError = true;
                    return false;
                }
            }
        } else if( (fc>='0' && fc<='9') || (fc>='A' && fc<='F') ) {
            // single permitted flag
            int fv = (fc>='0' && fc<='9') ? (fc - '0') : (fc - 'A' + 10);
            fm.permit(f, fv);
        } else {
            mError = true;
            return false;
        }
        f += 1;
        if( f == 16 ) {
            f = 0;
            add(fm);
            fm.reset();

            iflag >> fc;
            if( !iflag )
                break;
            if( fc != '|' ) {
                mError = true;
                return false;
            }
        }
    }
    return true;
}

std::string FlagSet::sql(const std::string& column) const
{
    if( mError )
        return "0=1";
    if( mMatchers.empty() )
        return mDefaultIfEmpty ? "" : "0=1";

    std::ostringstream sql;
    if( mMatchers.size()>1 )
        sql << '(';
    sql << mMatchers.front().sql(column);
    for(unsigned int i=1; i<mMatchers.size(); ++i)
        sql << " OR " << mMatchers[i].sql(column);
    if( mMatchers.size()>1 )
        sql << ')';
    return sql.str();
}
