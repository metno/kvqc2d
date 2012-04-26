
noinst_LIBRARIES = libhelpers.a

libhelpers_a_SOURCES = \
	Akima.cc \
	Akima.h \
	AkimaSpline.cc \
	AkimaSpline.h \
	AlgorithmHelpers.cc \
	AlgorithmHelpers.h \
	ConfigParser.cc \
	ConfigParser.h \
	FormulaUU.cc \
	FormulaUU.h \
	Helpers.cc \
	Helpers.h \
	WeightedMean.cc \
	WeightedMean.h \
	mathutil.cc \
	mathutil.h \
	stringutil.cc \
	stringutil.h \
	timeutil.cc \
	timeutil.h

AM_CXXFLAGS = -Wall -W -I$(top_srcdir)/src \
	$(PROG_DEPS_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)
