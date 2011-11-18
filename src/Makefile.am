
bin_PROGRAMS = kvqc2d

kvqc2d_SOURCES = Qc2Main.cc

noinst_LIBRARIES = libkvqc2d.a

libkvqc2d_a_SOURCES = \
	AlgorithmHelpers.cc \
	AlgorithmHelpers.h \
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
	debug.h \
	FlagChange.cc \
	FlagChange.h \
	FlagMatcher.cc \
	FlagMatcher.h \
	FlagSet.cc \
	FlagSet.h \
	FlagUpdater.cc \
	FlagUpdater.h \
	foreach.h \
	Helpers.cc \
	Helpers.h \
	InitLogger.cc \
	InitLogger.h \
	KvalobsDB.cc \
	KvalobsDB.h \
	AlgorithmDispatcher.cc \
	AlgorithmDispatcher.h \
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
	StandardBroadcaster.h \
	algorithms/AkimaSpline.cc \
	algorithms/AkimaSpline.h \
	algorithms/DataUpdate.cc \
	algorithms/DataUpdate.h \
	algorithms/DipTestAlgorithm.cc \
	algorithms/DipTestAlgorithm.h \
	algorithms/GapInterpolationAlgorithm.h \
	algorithms/GapInterpolationAlgorithm.cc \
	algorithms/GetStationParam.cc \
	algorithms/GetStationParam.h \
	algorithms/NeighborsDistance2.cc \
	algorithms/NeighborsDistance2.h \
	algorithms/ParseParValFile.cc \
	algorithms/ParseParValFile.h \
	algorithms/PlumaticAlgorithm.cc \
	algorithms/PlumaticAlgorithm.h \
	algorithms/RedistributionAlgorithm.cc \
	algorithms/RedistributionAlgorithm.h \
	algorithms/RedistributionNeighbors.cc \
	algorithms/RedistributionNeighbors.h \
	algorithms/SingleLinearAlgorithm.cc \
	algorithms/SingleLinearAlgorithm.h

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
