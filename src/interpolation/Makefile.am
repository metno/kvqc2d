
noinst_LIBRARIES = libinterpolation.a

libinterpolation_a_SOURCES = \
	InterpolationData.h \
	SingleParameterInterpolator.cc \
	SingleParameterInterpolator.h \
	NeighborInterpolator.cc \
	NeighborInterpolator.h \
	SimpleInterpolationResult.h \
	MinMaxInterpolator.cc \
	MinMaxInterpolator.h \
	KvalobsDataFilter.cc \
	KvalobsDataFilter.h \
	KvalobsNeighborData.cc \
	KvalobsNeighborData.h \
	KvalobsMinMaxData.cc \
	KvalobsMinMaxData.h \
	KvalobsDataUU.cc \
	KvalobsDataUU.h \
	Instrument.cc \
	Instrument.h \
	ParameterInfo.cc \
	ParameterInfo.h \
	GapInterpolationAlgorithm.cc \
	GapInterpolationAlgorithm.h

#	InterpolatorUU.cc \
#	InterpolatorUU.h \
#	CorrelatedNeighborInterpolator.cc \
#	CorrelatedNeighborInterpolator.h \
#	Instrument.cc \
#	Instrument.h \
#	SimpleData.cc \
#	SimpleData.h

AM_CXXFLAGS = -Wall -W \
	$(PROG_DEPS_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)
