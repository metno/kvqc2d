
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

bool endsWith(const std::string& text, const std::string& tail)
{
    int lText = text.length(), lTail = tail.length();
    return ( lText >= lTail && text.substr(lText-lTail) == tail );
}

bool startsWith(const std::string& text, const std::string& head)
{
    int lText = text.length(), lHead = head.length();
    return ( lText >= lHead && text.substr(0, lHead) == head );
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
