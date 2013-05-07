TESTS = kvqc2dTest corbatest

check_PROGRAMS = kvqc2dTest corbatest

corbatest_SOURCES= \
	dummy-kvserviced.cc

kvqc2dTest_SOURCES= \
	AkimaTest.cc \
	AlgorithmConfigTest.cc \
	ConfigParserTest.cc \
	FlagChangeTest.cc \
	FlagPatternsTest.cc \
	FlagPatternTest.cc \
	FlagUpdateTest.cc \
	FormulaUUTest.cc \
	HelpersTest.cc \
	algorithms/AggregatorLimitsTest.cc \
	algorithms/AkimaSplineTest.cc \
	algorithms/AlgorithmHelpersTest.cc \
	algorithms/AlgorithmTestBase.cc \
	algorithms/AlgorithmTestBase.h \
	algorithms/DataUpdateTest.cc \
	algorithms/DipTestTest.cc \
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
	interpolation/data_reproduce_kvalobs_18230.cc \
	interpolation/data_reproduce_kvalobs_68290.cc \
	interpolation/GapInterpolationTestBase.hh \
	interpolation/GapInterpolationTestBase.cc \
	interpolation/GIT_DoNotSetMaxTo100.cc \
	interpolation/GIT_FluctuationSize1.cc \
	interpolation/GIT_FluctuationSize2.cc \
	interpolation/GIT_GapInMultiplePar.cc \
	interpolation/GIT_InterpolationOutsideMinMax.cc \
	interpolation/GIT_MinMaxTooFar.cc \
	interpolation/GIT_MissingTASomeTAXN.cc \
	interpolation/GIT_NoInterpolationAtEnd.cc \
	interpolation/GIT_OnlyMissing.cc \
	interpolation/GIT_Overwritten.cc \
	interpolation/GIT_RABucketEmptied.cc \
	interpolation/GIT_reproduce_kvalobs_18230.cc \
	interpolation/GIT_reproduce_kvalobs_18950.cc \
	interpolation/GIT_reproduce_kvalobs_3290.cc \
	interpolation/GIT_reproduce_kvalobs_68290.cc \
	interpolation/GIT_reproduce_kvalobs_76926.cc \
	interpolation/GIT_reproduce_kvalobs_99740.cc \
	interpolation/GIT_SingleMissingTAXN.cc \
	interpolation/GIT_TAGroup.cc \
	interpolation/GIT_TAInterpolatedNeighbors.cc \
	interpolation/GIT_test1.cc \
	interpolation/GIT_UUBecomes0WithoutNieghbors.cc \
	interpolation/GIT_UUGroup.cc \
	interpolation/GIT_UUNoJump1.cc \
	interpolation/GIT_UUSeries.cc \
	interpolation/GIT_VeryLittleSnow.cc \
	test_main.cc

kvqc2dTest_DEPENDENCIES = algorithms/StatisticalMean_n110.icc algorithms/StatisticalMean_n212.icc \
	../src/libkvqc2d_1.a ../src/interpolation/libinterpolation.a \
	../src/statisticalmean/libstatisticalmean.a ../src/libkvqc2d_2.a \
	../src/helpers/libhelpers.a

AM_CXXFLAGS = -Wall -W \
	$(GTEST_CXXFLAGS) \
	$(TEST_DEPS_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)

#kvqc2dTest_CPPFLAGS += -ftest-coverage -fprofile-arcs
#kvqc2dTest_LDFLAGS  += -fprofile-arcs

INCLUDES = -I$(top_srcdir)/src

LDADD = \
	$(TEST_DEPS_LIBS) \
	$(GTEST_LIBS) \
	$(BOOST_THREAD_LIB) \
	$(BOOST_FILESYSTEM_LIB) -lboost_system

AM_LDFLAGS = $(GTEST_LDFLAGS)

kvqc2dTest_LDADD = $(LDADD) -lboost_regex \
	../src/libkvqc2d_1.a ../src/interpolation/libinterpolation.a \
	../src/statisticalmean/libstatisticalmean.a ../src/libkvqc2d_2.a \
	../src/helpers/libhelpers.a \
	$(LDADD)


if HAVE_COMPILED_GTEST
# nothing
else
noinst_LIBRARIES = libgtest_local.a
nodist_libgtest_local_a_SOURCES = $(GTEST_BASE)/src/gtest/src/gtest-all.cc
libgtest_local_a_CXXFLAGS = -I$(GTEST_BASE)/src/gtest

kvqc2dTest_LDADD += ./libgtest_local.a
endif

algorithms/StatisticalMeanTest.cc: algorithms/StatisticalMean_n110.icc algorithms/StatisticalMean_n212.icc

algorithms/StatisticalMean_n110.icc: algorithms/build_daymeans_110.py ../data/normals_110.dat
	python $^ $@

algorithms/StatisticalMean_n212.icc: algorithms/build_daymeans_212.py ../data/normals_212.dat
	python $^ $@
