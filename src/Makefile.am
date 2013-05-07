
GIT = git
VERSION_H  = version.h
VERSION_STAMP = $(VERSION_H:%.h=%.stamp)
BUILT_SOURCES  = $(VERSION_STAMP)
MAINTAINERCLEANFILES = $(VERSION_H)

bin_PROGRAMS = kvqc2d

kvqc2d_SOURCES = Qc2Main.cc

noinst_LIBRARIES = libkvqc2d_1.a libkvqc2d_2.a

libkvqc2d_1_a_SOURCES = \
	AlgorithmConfig.cc \
	AlgorithmConfig.h \
	AlgorithmDispatcher.cc \
	AlgorithmDispatcher.h \
	AlgorithmRunner.cc \
	AlgorithmRunner.h \
	Broadcaster.h \
	algorithms/AggregatorLimits.cc \
	algorithms/AggregatorLimits.h \
	algorithms/DataUpdate.cc \
	algorithms/DataUpdate.h \
	algorithms/DipTestAlgorithm.cc \
	algorithms/DipTestAlgorithm.h \
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
	InitLogger.cc \
	InitLogger.h \
	KvalobsDB.cc \
	KvalobsDB.h \
	KvalobsDbGate.cc \
	KvalobsDbGate.h \
	KvalobsElemExtract.h \
	KvServicedBroadcaster.cc \
	KvServicedBroadcaster.h \
	LogfileNotifier.cc \
	LogfileNotifier.h \
	SQLDataAccess.cc \
	SQLDataAccess.h \
	Qc2Algorithm.cc \
	Qc2Algorithm.h \
	Qc2App.cc \
	Qc2App.h

libkvqc2d_2_a_SOURCES = \
	Instrument.cc \
	Instrument.h \
	Notifier.cc \
	Notifier.h \
	Notifier.icc \
	TimeRange.cc \
	TimeRange.h

AM_CXXFLAGS = -Wall -W -I$(top_srcdir)/src \
	$(PROG_DEPS_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)

kvqc2d_LDFLAGS = -pthread -export-dynamic $(BOOST_LDFLAGS)

kvqc2d_LDADD = \
	libkvqc2d_1.a interpolation/libinterpolation.a statisticalmean/libstatisticalmean.a libkvqc2d_2.a helpers/libhelpers.a \
	$(PROG_DEPS_LIBS) \
	$(BOOST_THREAD_LIB) \
	$(BOOST_FILESYSTEM_LIB) -lboost_system -lgsl

#libkvqc2d_a_CPPFLAGS += -ftest-coverage -fprofile-arcs
#kvqc2d_LDFLAGS += -fprofile-arcs

ebrowse:
	ebrowse $(SOURCES)

$(VERSION_STAMP):
	@echo "#define KVQC2D_VERSION_FULL \"`$(top_srcdir)/version.sh`\"" > "$(VERSION_H).tmp"
	@if cmp -s "$(VERSION_H).tmp" "$(VERSION_H)"; then \
	    echo "Not updating '$(VERSION_H)'"; \
	    rm -f "$(VERSION_H).tmp"; \
	else \
	    echo "Updating '$(VERSION_H)'"; \
	    mv -f "$(VERSION_H).tmp" "$(VERSION_H)"; \
	fi
