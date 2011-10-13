
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

} // namespace Helpers
