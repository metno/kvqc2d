
noinst_LIBRARIES = libinterpolation.a

libinterpolation_a_SOURCES = \
	CorrelatedNeighborInterpolator.cc \
	CorrelatedNeighborInterpolator.h \
	Interpolator.cc \
	Interpolator.h \
	InterpolatorUU.cc \
	InterpolatorUU.h \
	KvalobsData.cc \
	KvalobsData.h \
	KvalobsDataUU.cc \
	KvalobsDataUU.h \
	MinMaxInterpolator.cc \
	MinMaxInterpolator.h \
	NeighborInterpolation.cc \
	NeighborInterpolation.h \
	SimpleData.cc \
	SimpleData.h

AM_CXXFLAGS = -Wall -W \
	$(PROG_DEPS_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)
