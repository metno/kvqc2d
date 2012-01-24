
#include "Helpers.h"

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

std::string datatext(const kvalobs::kvData& data)
{
    std::ostringstream out;
    out << "[stationid="    << data.stationID()
        << " AND obstime='" << data.obstime().isoTime() << '\''
        << " AND paramid="  << data.paramID()
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

int normalisedDayOfYear(const miutil::miDate& date)
{
    const int feb28 = miutil::miDate(date.year(), 2, 28).dayOfYear();
    int doy = date.dayOfYear();
    if( doy > feb28 ) {
        const int dec31 = miutil::miDate(date.year(), 12, 31).dayOfYear();
        doy -= dec31 - 365;
    }
    return doy;
}

} // namespace Helpers
