
noinst_LIBRARIES = libinterpolation.a

libinterpolation_a_SOURCES = \
	gdebug.hh \
	GapData.hh \
	GapData.cc \
	GapUpdate.hh \
	GapUpdate.cc \
	InterpolationData.h \
	InterpolationData.cc \
	InterpolationError.hh \
	SingleParameterInterpolator.cc \
	SingleParameterInterpolator.h \
	NeighborInterpolator.cc \
	NeighborInterpolator.h \
	MinMaxInterpolator.cc \
	MinMaxInterpolator.h \
	MinMaxReconstruction.cc \
	MinMaxReconstruction.h \
	KvalobsNeighborData.cc \
	KvalobsNeighborData.h \
	KvalobsMinMaxData.cc \
	KvalobsMinMaxData.h \
	ParameterInfo.cc \
	ParameterInfo.h \
	GapInterpolationAlgorithm.cc \
	GapInterpolationAlgorithm.h

AM_CXXFLAGS = -Wall -W -I$(top_srcdir)/src \
	$(PROG_DEPS_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)
