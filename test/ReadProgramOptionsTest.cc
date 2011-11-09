
#include <gtest/gtest.h>
#include "ReadProgramOptions.h"
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <fstream>

namespace fs = boost::filesystem;

TEST(ReadProgramOptionsTest, testScanNoDir)
{
    const char* no_dir = "/tmp/thisisnotaconfigurationfiledirectoryforqc2";

    ReadProgramOptions po;
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

TEST(ReadProgramOptionsTest, testScanEmptyDir)
{
    const char* dir = "/tmp/ReadProgramOptions_dir";
    fs::remove_all(dir);
    fs::create_directory(dir);

    ReadProgramOptions po;
    po.setConfigPath( dir );

    std::vector<std::string> files;
    files.push_back( "this should go away" );

    ASSERT_TRUE( fs::exists(dir) );
    ASSERT_TRUE( po.SelectConfigFiles(files) );
    ASSERT_TRUE( files.empty() );

    fs::remove_all(dir);
}

// ------------------------------------------------------------------------

TEST(ReadProgramOptionsTest, testScanMixedDir)
{
    const fs::path dir("/tmp/ReadProgramOptions_dir");
    fs::remove_all( dir );
    fs::create_directory( dir );

    fs::create_directory( dir / "directory.cfg" );
    fs::create_directory( dir / "another_directory" );
    fs::ofstream( (dir / "01match.cfg") ) << "x=y" << std::endl;
    fs::ofstream( (dir / "00match.cfg") ) << "x=z" << std::endl;
    fs::ofstream( (dir / "nomatch.nocfg") ) << "x=a" << std::endl;

    ReadProgramOptions po;
    po.setConfigPath( dir );

    std::vector<std::string> files;
    files.push_back( "this should go away" );

    ASSERT_TRUE( fs::exists(dir) );
    ASSERT_TRUE( po.SelectConfigFiles(files) );
    ASSERT_EQ( 2, files.size() );
    ASSERT_EQ( (dir / "00match.cfg").native_file_string(), files[0] );
    ASSERT_EQ( (dir / "01match.cfg").native_file_string(), files[1] );

    fs::remove_all( dir );
}

// ------------------------------------------------------------------------

TEST(ReadProgramOptionsTest, ParseFlagCU)
{
    std::stringstream config;
    config << "bad_uflags = heiho" << std::endl
           << "okay1_cflags = ___.ABC.DEF.___.|000.111.222.333." << std::endl
           << "okay2_cflags = ___.___.___.0123" << std::endl;
    ReadProgramOptions params;
    ASSERT_NO_THROW(params.Parse(config));

    FlagSetCU fcu;
    EXPECT_NO_THROW(params.getFlagSetCU(fcu, "okay1"));
    EXPECT_NO_THROW(params.getFlagSetCU(fcu, "okay2"));

    EXPECT_THROW(params.getFlagSetCU(fcu, "bad"), ConfigException);
}

// ------------------------------------------------------------------------

TEST(ReadProgramOptionsTest, ParseFlagChange)
{
    std::stringstream config;
    config << "bad1  = lalelu->boink" << std::endl
           << "bad2  = ___.ABC.DEF.___.|000.111.222.333." << std::endl
           << "okay1 = 012.345.678.ABC.->___.___.___.0123" << std::endl;
    ReadProgramOptions params;
    ASSERT_NO_THROW(params.Parse(config));

    FlagChange fc;
    EXPECT_THROW(params.getFlagChange(fc, "bad1"), ConfigException);
    EXPECT_THROW(params.getFlagChange(fc, "bad2"), ConfigException);

    EXPECT_NO_THROW(params.getFlagChange(fc, "okay1"));
}
