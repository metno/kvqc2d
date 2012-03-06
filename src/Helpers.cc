
#include "Helpers.h"

#include <boost/algorithm/string/trim.hpp>

#define NDEBUG 1
#include "debug.h"

namespace Helpers {

float round(float f)
{
    const float factor = 10;
    f *= factor;
    if( f < 0.0f )
        f -= 0.5;
    else
        f += 0.5;
    float ff = 0;
    modff(f, &ff);
    return ff / factor;
}

int hexCharToInt(char n)
{
    if (n >= '0' && n <= '9')
        return (n-'0');
    if (n >= 'A' && n <= 'F')
        return (n-'A'+10);
    // if (n >= 'a' && n <= 'f')
    //    return (n-'a'+10);
    return 0;
}

bool fillMapFromList(const std::string& list, std::map<int, float>& map, const char separator)
{
    map.clear();
    bool error = false;
    std::istringstream parse(list);
    while( parse ) {
        int key = 0;
        float value = 0;
        parse >> key >> value;
        if( !parse ) {
            error = true;
            break;
        }
        map[key] = value;
        char sep = ' ';
        parse >> sep;
        if( parse.eof() ) {
            break;
        } else if( sep != separator ) {
            error = true;
            break;
        }
    }
    if( error )
        map.clear();
    return !error;
}

// ------------------------------------------------------------------------

std::ostream& digits1(std::ostream& out)
{
    out << std::setiosflags(std::ios::fixed) << std::setprecision(1);
    return out;
}

// ------------------------------------------------------------------------

std::string datatext(const kvalobs::kvData& data, int hoursBefore)
{
    miutil::miTime start = data.obstime();
    if( hoursBefore > 0 )
        start.addHour( -hoursBefore );
    return datatext(data, start);
}

// ------------------------------------------------------------------------

std::string datatext(const kvalobs::kvData& data, const miutil::miTime& start)
{
    std::ostringstream out;
    out << "[stationid=" << data.stationID() << " AND ";
    if( start >= data.obstime() ) {
        out << "obstime='" << data.obstime().isoTime() << '\'';
    } else {
        out << "obstime BETWEEN '" << start.isoTime() << "' AND '" << data.obstime().isoTime() << '\'';
    }
    out << " AND paramid="  << data.paramID()
        << " AND typeid="   << data.typeID()
        << " AND sensor='"  << data.sensor() << "'"
        << " AND level="    << data.level()
        << "; original=" << digits1 << data.original()
        << " corr="        << digits1 << data.corrected()
        << " controlinfo="  << data.controlinfo().flagstring()
        << " cfailed='"     << data.cfailed() << "']";
    return out.str();
}

// ------------------------------------------------------------------------

Helpers::split2_t split2(const std::string& toSplit, const std::string& delimiter, bool trim)
{
    if( delimiter.empty() )
        throw std::runtime_error("empty delimiter");
    std::size_t pos = toSplit.find(delimiter);
    if( pos == std::string::npos )
        throw std::runtime_error("delimiter '" + delimiter + "' not found in '" + toSplit + "'");
    split2_t split(toSplit.substr(0, pos), toSplit.substr(pos+delimiter.size()));
    if( trim ) {
        boost::trim(split.first);
        boost::trim(split.second);
    }
    return split;
}

// ------------------------------------------------------------------------

Helpers::splitN_t splitN(const std::string& toSplit, const std::string& delimiter, bool trim)
{
    if( delimiter.empty() )
        throw std::runtime_error("empty delimiter");
    splitN_t split;
    std::size_t begin = 0;
    while( begin < toSplit.size() ) {
        DBGV(begin);
        std::size_t end = toSplit.find(delimiter, begin);
        DBGV(end);
        if( end == std::string::npos )
            end = toSplit.size();
        std::string part = toSplit.substr(begin, end-begin);
        DBGV(part)
        if( trim )
            boost::trim(part);
        split.push_back(part);
        begin = end+delimiter.size();
    }
    if( begin == toSplit.size() )
        split.push_back("");
    return split;
}

// ------------------------------------------------------------------------

int normalisedDayOfYear(const miutil::miDate& date)
{
    // February 29 is the same as February 28
    const int daysFromPreviousMonths[12] = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
    };
    int day = date.day(), month = date.month();
    if( month == 2 && day == 29 )
        day = 28;
    return daysFromPreviousMonths[month-1] + day;
}

} // namespace Helpers
