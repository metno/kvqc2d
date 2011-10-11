
#include <gtest/gtest.h>
#include "ReadProgramOptions.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <fstream>

namespace fs = boost::filesystem;

TEST(ReadProgramOptionsTest, testScanNoDir)
{
    const char* no_dir = "/tmp/thisisnotaconfgurationfiledirectoryforqc2";

    ReadProgramOptions po;
    po.setConfigPath( no_dir );

    std::vector<std::string> files;
    files.push_back( "this should go away" );

    ASSERT_FALSE( fs::exists(no_dir) );
    ASSERT_FALSE( po.SelectConfigFiles(files) );
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
