
#include "FlagSet.h"

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
    if( flagstring.empty() )
        return true;

    std::size_t lastSeparator = 0;
    while( lastSeparator < flagstring.size() ) {
        std::size_t nextSeparator = flagstring.find("|", lastSeparator);
        if( nextSeparator == std::string::npos )
            nextSeparator = flagstring.size();
        FlagMatcher fm;
        if( !fm.parse(flagstring.substr(lastSeparator, nextSeparator - lastSeparator)) ) {
            mError = true;
            return false;
        }
        add(fm);
        lastSeparator = nextSeparator + 1;
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

std::string FlagSetCU::sql() const
{
    const std::string sqlC = mControlflags.sql("controlinfo"), sqlU = mUseflags.sql("useinfo");
    if( sqlC.empty() )
        return sqlU;
    if( sqlC == "0=1" )
        return "0=1";
    return "(" + sqlC + " AND " + sqlU + ")";
}
