
#include "Helpers.h"

#include "scone.h"

namespace Helpers {

std::string kvqc2logstring(const kvalobs::kvData& kd)
{
    std::string logstring =
        StrmConvert(kd.stationID())+" "
        +StrmConvert(kd.obstime().year())+"-"
        +StrmConvert(kd.obstime().month())+"-"
        +StrmConvert(kd.obstime().day())+" "
        +StrmConvert(kd.obstime().hour())+":"
        +StrmConvert(kd.obstime().min())+":"
        +StrmConvert(kd.obstime().sec())+" "
        +StrmConvert(kd.original())+" "
        +StrmConvert(kd.paramID())+" "
        +StrmConvert(kd.typeID())+" "
        +StrmConvert(kd.sensor())+" "
        +StrmConvert(kd.level())+" "
        +StrmConvert(kd.corrected());

    return logstring;
}

bool string_endswith(const std::string& text, const std::string& tail)
{
    int lText = text.length(), lTail = tail.length();
    return ( lText >= lTail && text.substr(lText-lTail) == tail );
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


} // namespace Helpers
