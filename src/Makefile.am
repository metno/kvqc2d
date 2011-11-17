
bin_PROGRAMS = kvqc2d

kvqc2d_SOURCES = Qc2Main.cc

noinst_LIBRARIES = libkvqc2d.a

libkvqc2d_a_SOURCES = \
	AlgorithmHelpers.cc \
	AlgorithmHelpers.h \
	CheckedDataCommandBase.h \
	CheckedDataHelper.cc \
	CheckedDataHelper.h \
	config_bottom.h \
	ConfigParser.h \
	ConfigParser.cc \
	DBConstraintsBase.h \
	DBConstraints.cc \
	DBConstraints.h \
	DBInterface.cc \
	DBInterface.h \
	kvMetadataTable.cc \
	kvMetadataTable.h \
	foreach.h \
	FlagChange.cc \
	FlagChange.h \
	FlagMatcher.cc \
	FlagMatcher.h \
	FlagSet.cc \
	FlagSet.h \
	FlagUpdater.cc \
	FlagUpdater.h \
	Helpers.cc \
	Helpers.h \
	InitLogger.cc \
	InitLogger.h \
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
	scone.h \
	StandardBroadcaster.cc \
	StandardBroadcaster.h \
	StandardDB.cc \
	StandardDB.h \
	StopWatch.cc \
	StopWatch.h \
	algorithms/AkimaSpline.cc \
	algorithms/AkimaSpline.h \
	algorithms/BasicStatistics.h \
	algorithms/DataUpdate.cc \
	algorithms/DataUpdate.h \
	algorithms/DipTestAlgorithm.cc \
	algorithms/DipTestAlgorithm.h \
	algorithms/GapInterpolationAlgorithm.h \
	algorithms/GapInterpolationAlgorithm.cc \
	algorithms/GetStationParam.cc \
	algorithms/GetStationParam.h \
	algorithms/kvQABaseTypes.h \
	algorithms/NeighborsDistance2.cc \
	algorithms/NeighborsDistance2.h \
	algorithms/ParseParValFile.cc \
	algorithms/ParseParValFile.h \
	algorithms/Plumatic.cc \
	algorithms/Plumatic.h \
	algorithms/ProcessPlumatic.cc \
	algorithms/ProcessPlumatic.h \
	algorithms/proj++.cc \
	algorithms/proj++.h \
	algorithms/RedistributionAlgorithm.cc \
	algorithms/RedistributionAlgorithm.h \
	algorithms/RedistributionNeighbors.cc \
	algorithms/RedistributionNeighbors.h \
	algorithms/SingleLinearAlgorithm.cc \
	algorithms/SingleLinearAlgorithm.h \
	algorithms/StationSelection.cc \
	algorithms/StationSelection.h \
	algorithms/table_delaunay.cc \
	algorithms/table_delaunay.h \
	algorithms/tround.h 

AM_CXXFLAGS = -Wall -W \
	$(PROG_DEPS_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)

kvqc2d_LDFLAGS = -pthread -export-dynamic $(BOOST_LDFLAGS)

kvqc2d_LDADD = libkvqc2d.a \
	$(PROG_DEPS_LIBS) \
	$(BOOST_THREAD_LIB) \
	$(BOOST_FILESYSTEM_LIB)

#libkvqc2d_a_CPPFLAGS += -ftest-coverage -fprofile-arcs
#kvqc2d_LDFLAGS += -fprofile-arcs

clean-local:
	rm -rf test/var

ebrowse:
	ebrowse $(SOURCES)
