
noinst_LIBRARIES = libstatisticalmean.a

libstatisticalmean_a_SOURCES = \
	AccumulatedFloat.h \
	AccumulatedQuartiles.h \
	AccumulatedValue.cc \
	AccumulatedValue.h \
	Accumulator.cc \
	Accumulator.h \
	AccumulatorMeanOrSum.cc \
	AccumulatorMeanOrSum.h \
	AccumulatorQuartiles.cc \
	AccumulatorQuartiles.h \
	Checker.cc \
	Checker.h \
	CheckerMeanOrSum.cc \
	CheckerMeanOrSum.h \
	CheckerQuartiles.cc \
	CheckerQuartiles.h \
	DailyValueExtractor.cc \
	DailyValueExtractor.h \
	DayMean.cc \
	DayMean.h \
	Factory.cc \
	Factory.h \
	MeanFactory.cc \
	MeanFactory.h \
	QuartilesFactory.cc \
	QuartilesFactory.h \
	StatisticalMean.cc \
	StatisticalMean.h \
	SumFactory.cc \
	SumFactory.h

AM_CXXFLAGS = -Wall -W -I$(top_srcdir)/src \
	$(PROG_DEPS_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)
