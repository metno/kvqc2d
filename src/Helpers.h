
// c-*- c++ -*-

#ifndef Qc2Helpers_H
#define Qc2Helpers_H 1

#include <kvalobs/kvData.h>
#include <iosfwd>
#include <limits>
#include <map>
#include <string>

namespace Helpers {

float round(float f);

int hexCharToInt(char n);

bool fillMapFromList(const std::string& list, std::map<int, float>& map, const char separator = ',');

inline bool equal(float a,  float b)  { return fabs(a - b) < 4*std::numeric_limits<float> ::epsilon(); }
inline bool equal(double a, double b) { return fabs(a - b) < 4*std::numeric_limits<double>::epsilon(); }

std::ostream& digits1(std::ostream& out);

std::string datatext(const kvalobs::kvData& data);

/**
 * Returns the normalised day of the year, that is 28.2. = 29.2. = day
 * 59, 1.3. = 60, ...
 */
int normalisedDayOfYear(const miutil::miDate& date);

};

#endif
