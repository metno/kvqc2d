
// c-*- c++ -*-

#ifndef Qc2Helpers_H
#define Qc2Helpers_H 1

#include <kvalobs/kvData.h>
#include <algorithm>
#include <iosfwd>
#include <limits>
#include <map>
#include <string>

namespace Helpers {

float round1(float f);
float round(float f, float factor);

int hexCharToInt(char n);

bool fillMapFromList(const std::string& list, std::map<int, float>& map, const char separator = ',');

inline bool equal(float a,  float b)  { return fabs(a - b) < 4*std::numeric_limits<float> ::epsilon(); }
inline bool equal(double a, double b) { return fabs(a - b) < 4*std::numeric_limits<double>::epsilon(); }

bool isMissingOrRejected(const kvalobs::kvData& data);

std::ostream& digits1(std::ostream& out);

std::string datatext(const kvalobs::kvData& data, int hoursBefore=0);
std::string datatext(const kvalobs::kvData& data, const miutil::miTime& start);

typedef std::pair<std::string, std::string> split2_t;
split2_t split2(const std::string& toSplit, const std::string& delimiter, bool trim=false);

typedef std::vector<std::string> splitN_t;
splitN_t splitN(const std::string& toSplit, const std::string& delimiter, bool trim=false);

/**
 * Returns the normalised day of the year, that is 28.2. = 29.2. = day
 * 59, 1.3. = 60, ...
 */
int normalisedDayOfYear(const miutil::miDate& date);

template<class RandAccessIter>
double median(RandAccessIter& begin, RandAccessIter& end)
{
    // improved version of http://stackoverflow.com/questions/1719070/what-is-the-right-approach-when-using-stl-container-for-median-calculation
    if( begin == end )
        return 0;

    std::size_t size = end - begin;
    std::size_t idxM = size/2;
    RandAccessIter itM = begin + idxM;
    std::nth_element(begin, itM, end);

    double median = *itM;
    if( size % 2 != 0 ) {
        // odd number of elements
        begin = itM;
        end = itM + 1;
    } else {
        // even number of elements
        RandAccessIter itM0 = itM-1;
        std::nth_element(begin, itM0, itM);
        median = ( median + *itM0)/2.0;
        begin = end = itM;
    }
    return median;
}

template<class RandAccessIter>
double median(const RandAccessIter& begin, const RandAccessIter& end)
{
    RandAccessIter b = begin, e = end;
    return median(b, e);
}

template<class RandAccessIter>
void quartiles(RandAccessIter begin, RandAccessIter end, double& q1, double& q2, double& q3)
{
    RandAccessIter b = begin, e = end;
    q2 = median(begin, end);
    q1 = median(b, begin);
    q3 = median(end, e);
}

double randNormal();

template<typename T>
void maximize(T& value, const T& by)
{ if( by > value ) value = by; }

template<typename T>
void minimize(T& value, const T& by)
{ if( by < value ) value = by; }

template<typename T>
T limited_value(const T& value, const T& mini, const T& maxi)
{ if( value > maxi ) return maxi; else if( value < mini ) return mini; else return value; }

template<typename T>
void limit_value(T& value, const T& mini, const T& maxi)
{ value = limited_value(value); }

} // namespace Helpers

#endif
