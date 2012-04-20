/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 Copyright (C) 2007-2012 met.no

 Contact information:
 Norwegian Meteorological Institute
 Box 43 Blindern
 0313 OSLO
 NORWAY
 email: kvalobs-dev@met.no

 This file is part of KVALOBS

 KVALOBS is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 KVALOBS is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License along
 with KVALOBS; if not, write to the Free Software Foundation Inc.,
 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/****************************************************************
 **
 ** Implementation ParseParValFile class
 **
 ****************************************************************/

#include "ParseParValFile.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

ParseParValFile::ParseParValFile(std::string filename)
{
    std::ifstream ind;
    ind.open(filename.c_str());

    if (ind) {
        while (!ind.eof()) {
            float valis;
            int key;
            ind >> key;
            ind >> valis;
            ParValMap[key] = valis;
        }
    } else {
        std::cout << "Could not open paramid and value file!" << std::endl;
    }

    ind.close();
}

std::map<int, float> ParseParValFile::ReturnMap()
{
    return ParValMap;
}

