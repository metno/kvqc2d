
bin_PROGRAMS = kvqc2d

kvqc2d_SOURCES = Qc2Main.cc

noinst_LIBRARIES = libkvqc2d.a

libkvqc2d_a_SOURCES = \
	DBConstraints.cc \
	DBConstraints.h \
	DBInterface.cc \
	DBInterface.h \
	CheckedDataCommandBase.h \
	CheckedDataHelper.cc \
	CheckedDataHelper.h \
	kvMetadataTable.cc \
	kvMetadataTable.h \
	ConfigParser.h \
	ConfigParser.cc \
	InitLogger.cc \
	InitLogger.h \
	AlgorithmHelpers.cc \
	AlgorithmHelpers.h \
	Helpers.cc \
	Helpers.h \
	FlagMatcher.cc \
	FlagMatcher.h \
	FlagSet.cc \
	FlagSet.h \
	ProcessControl.cc \
	ProcessControl.h \
	ProcessImpl.cc \
	ProcessImpl.h \
	Qc2Algorithm.cc \
	Qc2Algorithm.h \
	Qc2App.cc \
	Qc2App.h \
	Qc2Connection.h \
	Qc2Connection.cc \
	Qc2Thread.cc \
	Qc2Thread.h \
	ReadProgramOptions.cc \
	ReadProgramOptions.h \
	StandardBroadcaster.cc \
	StandardBrodacaster.h \
	StandardDB.cc \
	StandardDB.h \
	StopWatch.cc \
	StopWatch.h \
	algorithms/BasicStatistics.h \
	algorithms/Plumatic.cc \
	algorithms/Plumatic.h \
	algorithms/GetStationParam.cc \
	algorithms/GetStationParam.h \
	algorithms/kvQABaseTypes.h \
	algorithms/RedistributionAlgorithm.cc \
	algorithms/RedistributionAlgorithm.hh \
	algorithms/SingleLinearAlgorithm.cc \
	algorithms/SingleLinearAlgorithm.hh \
	algorithms/GapInterpolationAlgorithm.cc \
	algorithms/GapInterpolationAlgorithm.hh \
	algorithms/ProcessPlumatic.cc \
	algorithms/ProcessPlumatic.hh \
	algorithms/DipTestAlgorithm.cc \
	algorithms/DipTestAlgorithm.hh \
	algorithms/proj++.cc \
	algorithms/proj++.h \
	algorithms/Qc2D.cc \
	algorithms/Qc2D.h \
	algorithms/Qc3D.cc \
	algorithms/Qc3D.h \
	algorithms/ParseParValFile.cc \
	algorithms/ParseParValFile.h \
	algorithms/StationSelection.cc \
	algorithms/StationSelection.h \
	algorithms/table_delaunay.cc \
	algorithms/table_delaunay.h \
	algorithms/AkimaSpline.cc \
	algorithms/AkimaSpline.h \
	algorithms/scone.h \
	algorithms/tround.h 

AM_CXXFLAGS = -Wall -W \
	$(PROG_DEPS_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)
	$(omniDynamic4_CFLAGS)

kvqc2d_LDFLAGS = -pthread -export-dynamic $(BOOST_LDFLAGS)

kvqc2d_LDADD = libkvqc2d.a \
	$(PROG_DEPS_LIBS) \
	$(BOOST_THREAD_LIB) \
	$(BOOST_REGEX_LIB) \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(omniDynamic4_LIBS) \
        -lnetcdf_c++

#libkvqc2d_a_CPPFLAGS += -ftest-coverage -fprofile-arcs
#kvqc2d_LDFLAGS += -fprofile-arcs

clean-local:
	rm -rf test/var


#sqldir=$(datadir)/kvsynopd
#sql_DATA=kvsynopd_schema.sql
#bin_SCRIPTS = kvsynopd_initdb
