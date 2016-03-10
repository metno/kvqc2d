/* -*- c++ -*-
 Kvalobs - Free Quality Control Software for Meteorological Observations

 Copyright (C) 2016 met.no

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
#ifndef KVQC2D_SINGLEFILELOGSTREAM_H
#define KVQC2D_SINGLEFILELOGSTREAM_H

#include <milog/LogStream.h>
#include <fstream>

/**
 * \brief A log stream to log to a single file, without rotating.
 */
class SingleFileLogStream : public milog::LogStream {
public:
    /**
     * \brief Use StdLayout.
     */
    SingleFileLogStream(const std::string& filename);

    ~SingleFileLogStream();

    /**
     * \brief check if the file is open.
     */
    bool is_open()
        { return out_.is_open(); }

    /**
     * \brief Close this LogStream.
     */
    void close()
        { out_.close(); }

protected:
    void write(const std::string &message);

private:
    std::ofstream out_;
};

#endif // KVQC2D_SINGLEFILELOGSTREAM_H
