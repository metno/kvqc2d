
#include "ConfigParser.h"
#include <sstream>
#include <gtest/gtest.h>

TEST(ConfigParserTest, testOk)
{
    std::stringstream io;
    io << "key1 = 123" << std::endl
       << "# comment" << std::endl
       << "key2= salmon" << std::endl
       << "key2 =dolphin" << std::endl
       << "key3 =====" << std::endl;

    ConfigParser c;
    ASSERT_TRUE( c.load(io) );

    ASSERT_TRUE( c.has("key1") );
    ASSERT_EQ( 1, c.get("key1").count() );
    ASSERT_EQ( "123", c.get("key1").convert<std::string>(0) );
    ASSERT_EQ( 123, c.get("key1").convert<int>(0, -1) );

    ASSERT_TRUE( c.has("key2") );
    ASSERT_EQ( 2, c.get("key2").count() );
    ASSERT_EQ( "salmon", c.get("key2").convert<std::string>(0) );
    ASSERT_EQ( "dolphin", c.get("key2").convert<std::string>(1) );

    ASSERT_EQ( 1, c.get("key3").count() );
    ASSERT_EQ( "====", c.get("key3").convert<std::string>(0) );
}

// ------------------------------------------------------------------------

TEST(ConfigParserTest, testConvertFail)
{
    std::stringstream io;
    io << "# key1 = 123" << std::endl
       << "key1 = onetwothree" << std::endl;

    ConfigParser c;
    ASSERT_TRUE( c.load(io) );

    ASSERT_TRUE( c.has("key1") );
    ASSERT_EQ( 1, c.get("key1").count() );
    ASSERT_EQ( "onetwothree", c.get("key1").convert<std::string>(0) );
    ASSERT_EQ( -1, c.get("key1").convert<int>(0, -1) );
}

// ------------------------------------------------------------------------

TEST(ConfigParserTest, testParseErrors)
{
    std::stringstream io;
    io << "  # comment" << std::endl
       << "" << std::endl
       << "# next line only has spaces" << std::endl
       << "    " << std::endl
       << "=1" << std::endl
       << "hallo" << std::endl
       << "=bert=brot" << std::endl
       << "wurm=" << std::endl
       << "roger#=alles ok" << std::endl
       << "#bert=#vogelmann" << std::endl
       << "bert=#vogelmann#" << std::endl;

    ConfigParser c;
    ASSERT_FALSE( c.load(io) );

    ASSERT_TRUE( c.has("bert") );
    ASSERT_EQ( 1, c.get("bert").count() );
    ASSERT_EQ( "#vogelmann#", c.get("bert").convert<std::string>(0) );

    ASSERT_EQ( 5, c.errors().size() );
}

// ------------------------------------------------------------------------

TEST(ConfigParserTest, testConvertError)
{
    std::stringstream io;
    io << "float=123.4" << std::endl
       << "int=123" << std::endl
       << "chars=ab" << std::endl;

    ConfigParser c;
    ASSERT_TRUE( c.load(io) );

    ASSERT_TRUE( c.has("float") );
    ASSERT_EQ( 1, c.get("float").count() );
    ASSERT_EQ( "123.4", c.get("float").convert<std::string>(0) );
    ASSERT_FLOAT_EQ( 123.4, c.get("float").convert<float>(0, -1) );
    ASSERT_EQ( -1, c.get("float").convert<int>(0, -1) );

    ASSERT_TRUE( c.has("int") );
    ASSERT_EQ( 1, c.get("int").count() );
    ASSERT_EQ( "123", c.get("int").convert<std::string>(0) );
    ASSERT_FLOAT_EQ( 123, c.get("int").convert<float>(0, -1) );
    ASSERT_EQ( 123, c.get("int").convert<int>(0, -1) );

    ASSERT_TRUE( c.has("chars") );
    ASSERT_EQ( 1, c.get("chars").count() );
    ASSERT_EQ( "ab", c.get("chars").convert<std::string>(0) );
    ASSERT_EQ( '?', c.get("chars").convert<char>(0, '?') );
}

// ------------------------------------------------------------------------

TEST(ConfigParserTest, testConvertList)
{
    std::stringstream io;
    io << "int=1" << std::endl
       << "int=2" << std::endl
       << "int=3" << std::endl;

    ConfigParser c;
    ASSERT_TRUE( c.load(io) );

    ASSERT_TRUE( c.has("int") );
    std::vector<int> v = c.get("int").convert<int>();
    ASSERT_EQ( 3, v.size() );
    ASSERT_EQ( 1, v[0] );
    ASSERT_EQ( 2, v[1] );
    ASSERT_EQ( 3, v[2] );
}

// ------------------------------------------------------------------------

TEST(ConfigParserTest, testConvertListFail)
{
    std::stringstream io;
    io << "int=1" << std::endl
       << "int=2" << std::endl
       << "int=no number" << std::endl;

    ConfigParser c;
    ASSERT_TRUE( c.load(io) );

    ASSERT_TRUE( c.has("int") );
    std::vector<int> v = c.get("int").convert<int>();
    ASSERT_TRUE( v.empty() );
}

// ------------------------------------------------------------------------

TEST(ConfigParserTest, testWSAroundValue)
{
    std::stringstream io;
    io << "int = 12" << std::endl
       << "int =24 " << std::endl;

    ConfigParser c;
    ASSERT_TRUE( c.load(io) );

    ASSERT_TRUE( c.has("int") );
    std::vector<int> v = c.get("int").convert<int>();
    ASSERT_EQ( 2, v.size() );
    ASSERT_EQ( 12, v[0] );
    ASSERT_EQ( 24, v[1] );
}

// ------------------------------------------------------------------------

TEST(ConfigParserTest, ValueOnlyWS)
{
    std::stringstream io;
    io << "boa = "   << std::endl;

    ConfigParser c;
    ASSERT_FALSE( c.load(io) );
}

// ------------------------------------------------------------------------

TEST(ConfigParserTest, UnusedOptions)
{
    std::stringstream io;
    io << "used1 = yes\n"
       << "used2 = 1\n"
       << "used2 = 2\n"
       << "used2 = 3\n"
       << "used3 = a\n"
       << "used3 = b\n"
       << "unused1 = oh\n"
       << "unused1 = oh\n"
       << "unused2 = bad thing\n";

    ConfigParser c;
    ASSERT_TRUE( c.load(io) );

    std::string used1 = c.get("used1").convert<std::string>(0);
    std::vector<int> used2 = c.get("used2").convert<int>();
    std::vector<std::string> used3 = c.get("used3").convert<std::string>();

    ErrorList e = c.checkUnrequested();
    ASSERT_EQ(2, e.size());
}
