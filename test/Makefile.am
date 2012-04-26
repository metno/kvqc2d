TESTS = kvqc2dTest corbatest

check_PROGRAMS = kvqc2dTest corbatest

corbatest_SOURCES= \
	dummy-kvserviced.cc

kvqc2dTest_SOURCES= \
	AkimaTest.cc \
	AlgorithmConfigTest.cc \
	algorithms/AkimaSplineTest.cc \
	algorithms/AlgorithmHelpersTest.cc \
	algorithms/AlgorithmTestBase.cc \
	algorithms/AlgorithmTestBase.h \
	algorithms/DataUpdateTest.cc \
	algorithms/DipTestTest.cc \
	algorithms/GapInterpolationTest.cc \
	algorithms/PlumaticTest.cc \
	algorithms/RedistributionTest.cc \
	algorithms/SingleLinearTest.cc \
	algorithms/TestBroadcaster.cc \
	algorithms/TestBroadcaster.h \
	algorithms/TestData.cc \
	algorithms/TestData.h \
	algorithms/TestDB.cc \
	algorithms/TestDB.h \
	algorithms/TestNotifier.cc \
	algorithms/TestNotifier.h \
	algorithms/StatisticalMeanTest.cc \
	ConfigParserTest.cc \
	FlagChangeTest.cc \
	FlagPatternsTest.cc \
	FlagPatternTest.cc \
	FlagUpdateTest.cc \
	FormulaUUTest.cc \
	HelpersTest.cc \
        test_main.cc

AM_CXXFLAGS = -Wall -W \
	$(TEST_DEPS_CFLAGS) \
	$(GTEST_CXXFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)

#kvqc2dTest_CPPFLAGS += -ftest-coverage -fprofile-arcs
#kvqc2dTest_LDFLAGS  += -fprofile-arcs

INCLUDES = -I$(top_srcdir)/src

LDADD = \
	$(TEST_DEPS_LIBS) \
	$(GTEST_LIBS) \
	$(BOOST_THREAD_LIB) \
	$(BOOST_FILESYSTEM_LIB)

kvqc2dTest_LDADD = $(LDADD) -lboost_regex \
	../src/libkvqc2d.a ../src/interpolation/libinterpolation.a \
	../src/statisticalmean/libstatisticalmean.a ../src/TimeRange.o \
	../src/helpers/libhelpers.a

kvqc2dTest_LDFLAGS = $(GTEST_LDFLAGS)


algorithms/StatisticalMeanTest.cc: algorithms/StatisticalMean_n110.icc algorithms/StatisticalMean_n212.icc

algorithms/StatisticalMean_n110.icc: algorithms/build_daymeans_110.py ../data/normals_110.dat
	python $^ $@

algorithms/StatisticalMean_n212.icc: algorithms/build_daymeans_212.py ../data/normals_212.dat
	python $^ $@
