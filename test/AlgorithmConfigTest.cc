/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011-2012 met.no

  Contact information:
  Norwegian Meteorological Institute
  Postboks 43 Blindern
  N-0313 OSLO
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

#include <gtest/gtest.h>
#include "AlgorithmConfig.h"
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <fstream>

namespace fs = boost::filesystem;

namespace {
#if BOOST_FILESYSTEM_VERSION >= 3
    inline std::string to_native_file(const boost::filesystem::path& path) {
        return path.native();
    }
    inline std::string to_native_dir(const boost::filesystem::path& path) {
        return path.native();
    }
#else
    inline std::string to_native_file(const boost::filesystem::path& path) {
        return path.native_file_string();
    }
    inline std::string to_native_dir(const boost::filesystem::path& path) {
        return path.native_directory_string();
    }
#endif
}

TEST(AlgorithmConfigTest, testScanNoDir)
{
    const char* no_dir = "/tmp/thisisnotaconfigurationfiledirectoryforqc2";

    AlgorithmConfig po;
    po.setConfigPath( no_dir );

    std::vector<std::string> files;
    files.push_back( "this should go away" );

    try {
        ASSERT_FALSE( fs::exists(no_dir) );
    } catch(fs::filesystem_error& fse) {
        FAIL() << "caught filesystem_error '" << fse.what() << "' on exists";
    }
    try {
        ASSERT_FALSE( po.SelectConfigFiles(files) );
    } catch(fs::filesystem_error& fse) {
        FAIL() << "caught filesystem_error '" << fse.what() << "' on SelectConfigFiles";
    }
    ASSERT_TRUE( files.empty() );
}

// ------------------------------------------------------------------------

TEST(AlgorithmConfigTest, testScanEmptyDir)
{
    const char* dir = "/tmp/ReadProgramOptions_dir";
    fs::remove_all(dir);
    fs::create_directory(dir);

    AlgorithmConfig po;
    po.setConfigPath( dir );

    std::vector<std::string> files;
    files.push_back( "this should go away" );

    ASSERT_TRUE( fs::exists(dir) );
    ASSERT_TRUE( po.SelectConfigFiles(files) );
    ASSERT_TRUE( files.empty() );

    fs::remove_all(dir);
}

// ------------------------------------------------------------------------

TEST(AlgorithmConfigTest, testScanMixedDir)
{
    const fs::path dir("/tmp/ReadProgramOptions_dir");
    fs::remove_all( dir );
    fs::create_directory( dir );

    fs::create_directory( dir / ("directory" + AlgorithmConfig::CFG_EXT) );
    fs::create_directory( dir / "another_directory" );
    fs::ofstream( (dir / ("01match" + AlgorithmConfig::CFG_EXT)) ) << "x=y" << std::endl;
    fs::ofstream( (dir / ("00match" + AlgorithmConfig::CFG_EXT)) ) << "x=z" << std::endl;
    fs::ofstream( (dir / ("nomatch" + AlgorithmConfig::CFG_EXT + "no")) ) << "x=a" << std::endl;

    AlgorithmConfig po;
    po.setConfigPath( dir );

    std::vector<std::string> files;
    files.push_back( "this should go away" );

    ASSERT_TRUE( fs::exists(dir) );
    ASSERT_TRUE( po.SelectConfigFiles(files) );
    ASSERT_EQ( 2, files.size() );
    ASSERT_EQ( to_native_file(dir / ("00match" + AlgorithmConfig::CFG_EXT)), files[0] );
    ASSERT_EQ( to_native_file(dir / ("01match" + AlgorithmConfig::CFG_EXT)), files[1] );
    
    fs::remove_all( dir );
}

// ------------------------------------------------------------------------

TEST(AlgorithmConfigTest, ParseFlagCU)
{
    std::stringstream config;
    config << "bad_uflags = heiho" << std::endl
           << "okay1_cflags = ___.ABC.DEF.___.|000.111.222.333." << std::endl
           << "okay2_cflags = ___.___.___.0123" << std::endl;
    AlgorithmConfig params;
    ASSERT_NO_THROW(params.Parse(config));

    FlagSetCU fcu;
    EXPECT_NO_THROW(params.getFlagSetCU(fcu, "okay1"));
    EXPECT_NO_THROW(params.getFlagSetCU(fcu, "okay2"));

    EXPECT_THROW(params.getFlagSetCU(fcu, "bad"), ConfigException);
}

// ------------------------------------------------------------------------

TEST(AlgorithmConfigTest, ParseFlagChange)
{
    std::stringstream config;
    config << "bad1  = lalelu->boink" << std::endl
           << "bad2  = ___.ABC.DEF.___.|000.111.222.333." << std::endl
           << "okay1 = 012.345.678.ABC.->___.___.___.0123" << std::endl;
    AlgorithmConfig params;
    ASSERT_NO_THROW(params.Parse(config));

    FlagChange fc;
    EXPECT_THROW(params.getFlagChange(fc, "bad1"), ConfigException);
    EXPECT_THROW(params.getFlagChange(fc, "bad2"), ConfigException);
    EXPECT_THROW(params.getFlagChange(fc, "bad3", "fd=7;fmis=3->fmis01;fmis=0->fmis=4"), ConfigException);

    EXPECT_NO_THROW(params.getFlagChange(fc, "okay1"));
}

// ------------------------------------------------------------------------

TEST(AlgorithmConfigTest, StartHH)
{
    std::stringstream config;
    config << "Last_NDays = 4\n"
           << "Start_hh = 06\n";
    AlgorithmConfig params;
    ASSERT_NO_THROW(params.Parse(config));

    ASSERT_EQ(6, params.UT0.hour());
    ASSERT_EQ(6, params.UT1.hour());
    ASSERT_EQ(4*24, miutil::miTime::hourDiff(params.UT1, params.UT0));
}
