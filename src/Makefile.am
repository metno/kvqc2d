
GIT = git
VERSION_GIT_H  = version_git.h
VERSION_GIT_STAMP = $(VERSION_GIT_H:%.h=%.stamp)
BUILT_SOURCES  = $(VERSION_GIT_STAMP)
MAINTAINERCLEANFILES = $(VERSION_GIT_H)

bin_PROGRAMS = kvqc2d

kvqc2d_SOURCES = Qc2Main.cc

noinst_LIBRARIES = libkvqc2d.a

libkvqc2d_a_SOURCES = \
	Akima.cc \
	Akima.h \
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
	algorithms/StatisticalMean.cc \
	algorithms/StatisticalMean.h \
	config_bottom.h \
	ConfigParser.cc \
	ConfigParser.h \
	CorrelatedNeighborInterpolator.cc \
	CorrelatedNeighborInterpolator.h \
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
	FormulaUU.cc \
	FormulaUU.h \
	Helpers.cc \
	Helpers.h \
	InitLogger.cc \
	InitLogger.h \
	Interpolator.cc \
	Interpolator.h \
	KvalobsDB.cc \
	KvalobsDB.h \
	KvalobsDbGate.cc \
	KvalobsDbGate.h \
	KvalobsElemExtract.h \
	KvServicedBroadcaster.cc \
	KvServicedBroadcaster.h \
	LogfileNotifier.cc \
	LogfileNotifier.h \
	MinMaxInterpolator.cc \
	MinMaxInterpolator.h \
	NeighborInterpolation.cc \
	NeighborInterpolation.h \
	Notifier.cc \
	Notifier.h \
	SQLDataAccess.cc \
	SQLDataAccess.h \
	Qc2Algorithm.cc \
	Qc2Algorithm.h \
	Qc2App.cc \
	Qc2App.h

AM_CXXFLAGS = -Wall -W \
	$(PROG_DEPS_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)

kvqc2d_LDFLAGS = -pthread -export-dynamic $(BOOST_LDFLAGS)

kvqc2d_LDADD = $(PROG_DEPS_LIBS) \
	$(BOOST_THREAD_LIB) \
	$(BOOST_FILESYSTEM_LIB) \
	libkvqc2d.a

#libkvqc2d_a_CPPFLAGS += -ftest-coverage -fprofile-arcs
#kvqc2d_LDFLAGS += -fprofile-arcs

ebrowse:
	ebrowse $(SOURCES)

$(VERSION_GIT_STAMP):
	@if $(GIT) rev-parse HEAD >/dev/null 2>&1; then \
	    $(GIT) update-index --refresh > /dev/null; \
	    GIT_REF="$$($(GIT) rev-parse HEAD)"; \
	    GIT_DIFF_FILES="$$($(GIT) diff-files)"; \
	    GIT_DIFF_INDEX="$$($(GIT) diff-index --cached HEAD)"; \
	    if [ "x$$GIT_DIFF_FILES" = "x" ] && [ "x$$GIT_DIFF_INDEX" = "x" ]; then \
	        GIT_VERSION=" (git:$$GIT_REF/clean)"; \
	    else \
	        GIT_VERSION=" (git:$$GIT_REF/dirty)"; \
	    fi; \
	elif grep -q "(git:.*)" "$(VERSION_GIT_H)"; then \
	    GIT_VERSION="`sed -e 's,^.*(git:\([a-f0-9]\+/[a-z]\+\)\( ?\)\?).*$$, (git:\1 ?),' \"$(VERSION_GIT_H)\"`"; \
	else \
	    GIT_VERSION=""; \
	fi; \
	echo "#define KVQC2D_VERSION_GIT \"$$GIT_VERSION\"" > "$(VERSION_GIT_H).tmp"
	@if cmp -s "$(VERSION_GIT_H).tmp" "$(VERSION_GIT_H)"; then \
	    echo "Not updating '$(VERSION_GIT_H)'"; \
	    rm -f "$(VERSION_GIT_H).tmp"; \
	else \
	    echo "Updating '$(VERSION_GIT_H)'"; \
	    mv -f "$(VERSION_GIT_H).tmp" "$(VERSION_GIT_H)"; \
	fi
