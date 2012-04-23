
// c-*- c++ -*-

#ifndef Qc2Helpers_H
#define Qc2Helpers_H 1

#include <kvalobs/kvData.h>
#include <iosfwd>
#include <map>
#include <string>

namespace Helpers {

int hexCharToInt(char n);

bool fillMapFromList(const std::string& list, std::map<int, float>& map, const char separator = ',');

bool isMissingOrRejected(const kvalobs::kvData& data);

std::ostream& digits1(std::ostream& out);

std::string datatext(const kvalobs::kvData& data, int hoursBefore=0);
std::string datatext(const kvalobs::kvData& data, const miutil::miTime& start);

} // namespace Helpers

#endif
