
// c-*- c++ -*-

#ifndef Qc2Helpers_H
#define Qc2Helpers_H 1

#include <kvalobs/kvData.h>
#include <map>
#include <string>

namespace Helpers {

std::string kvqc2logstring(const kvalobs::kvData& kd);

bool endsWith(const std::string& text, const std::string& tail);

inline bool string_endswith(const std::string& text, const std::string& tail)
    { return endsWith(text, tail); }

bool startsWith(const std::string& text, const std::string& head);

int hexCharToInt(char n);

bool fillMapFromList(const std::string& list, std::map<int, float>& map, const char separator = ',');

inline bool equal(float a,  float b)  { return std::fabs(a - b) < 4*std::numeric_limits<float> ::epsilon(); }
inline bool equal(double a, double b) { return std::fabs(a - b) < 4*std::numeric_limits<double>::epsilon(); }

};

#endif
