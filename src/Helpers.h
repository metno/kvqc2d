
// c-*- c++ -*-

#ifndef Qc2Helpers_H
#define Qc2Helpers_H 1

#include <kvalobs/kvData.h>
#include <string>

namespace Helpers {

std::string kvqc2logstring(const kvalobs::kvData& kd);

bool string_endswith(const std::string& text, const std::string& tail);

int hexCharToInt(char n);

};

#endif
