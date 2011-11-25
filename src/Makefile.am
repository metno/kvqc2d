
bin_PROGRAMS = kvqc2d

kvqc2d_SOURCES = Qc2Main.cc

noinst_LIBRARIES = libkvqc2d.a

libkvqc2d_a_SOURCES = \
	AlgorithmConfig.cc \
	AlgorithmConfig.h \
	AlgorithmDispatcher.cc \
	AlgorithmDispatcher.h \
	AlgorithmHelpers.cc \
	AlgorithmHelpers.h \
	AlgorithmRunner.cc \
	AlgorithmRunner.h \
	Broadcaster.h \
	algorithms/AkimaSpline.cc \
	algorithms/AkimaSpline.h \
	algorithms/DataUpdate.cc \
	algorithms/DataUpdate.h \
	algorithms/DipTestAlgorithm.cc \
	algorithms/DipTestAlgorithm.h \
	algorithms/GapInterpolationAlgorithm.cc \
	algorithms/GapInterpolationAlgorithm.h \
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
	algorithms/SingleLinearAlgorithm.h \
	config_bottom.h \
	ConfigParser.cc \
	ConfigParser.h \
	DBConstraintsBase.h \
	DBConstraints.cc \
	DBConstraints.h \
	DBInterface.cc \
	DBInterface.h \
	debug.h \
	FlagChange.cc \
	FlagChange.h \
	FlagPattern.cc \
	FlagPattern.h \
	FlagPatterns.cc \
	FlagPatterns.h \
	FlagUpdate.cc \
	FlagUpdate.h \
	foreach.h \
	Helpers.cc \
	Helpers.h \
	InitLogger.cc \
	InitLogger.h \
	KvalobsDB.cc \
	KvalobsDB.h \
	KvServicedBroadcaster.cc \
	KvServicedBroadcaster.h \
	LogfileNotifier.cc \
	LogfileNotifier.h \
	Notifier.cc \
	Notifier.h \
	Qc2Algorithm.cc \
	Qc2Algorithm.h \
	Qc2App.cc \
	Qc2App.h

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
