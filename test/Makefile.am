TESTS = kvqc2dTest

check_PROGRAMS = kvqc2dTest

kvqc2dTest_SOURCES= \
	AlgorithmConfigTest.cc \
	algorithms/AkimaSplineTest.cc \
	algorithms/AlgorithmHelpersTest.cc \
	algorithms/AlgorithmTestBase.cc \
	algorithms/AlgorithmTestBase.h \
	algorithms/DipTestTest.cc \
	algorithms/PlumaticTest.cc \
	algorithms/RedistributionTest.cc \
	algorithms/SingleLinearTest.cc \
	ConfigParserTest.cc \
	FlagChangeTest.cc \
	FlagPatternsTest.cc \
	FlagPatternTest.cc \
	FlagUpdateTest.cc \
	HelpersTest.cc \
        test_main.cc

AM_CXXFLAGS = -Wall -W \
	$(TEST_DEPS_CFLAGS) \
	$(GTEST_CXXFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)

#kvqc2dTest_CPPFLAGS += -ftest-coverage -fprofile-arcs
#kvqc2dTest_LDFLAGS  += -fprofile-arcs

kvqc2dTest_LDADD = ../src/libkvqc2d.a \
	$(TEST_DEPS_LIBS) \
	$(GTEST_LIBS) \
	$(BOOST_THREAD_LIB) \
	$(BOOST_FILESYSTEM_LIB)

kvqc2dTest_LDFLAGS = $(GTEST_LDFLAGS) 
