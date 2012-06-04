
include_directories(.. ../helpers)

########## statisticalmean algorithm static library ##########

SET(statisticalmean_STAT_SRCS
   AccumulatedFloat.h
   AccumulatedQuartiles.h
   AccumulatedValue.cc
   AccumulatedValue.h
   Accumulator.cc
   Accumulator.h
   AccumulatorMeanOrSum.cc
   AccumulatorMeanOrSum.h
   AccumulatorQuartiles.cc
   AccumulatorQuartiles.h
   Checker.cc
   Checker.h
   CheckerMeanOrSum.cc
   CheckerMeanOrSum.h
   CheckerQuartiles.cc
   CheckerQuartiles.h
   DayMean.h
   DayMeanExtractor.cc
   DayMeanExtractor.h
   DayValue.cc
   DayValue.h
   DayValueExtractor.cc
   DayValueExtractor.h
   Factory.cc
   Factory.h
   MeanFactory.cc
   MeanFactory.h
   QuartilesFactory.cc
   QuartilesFactory.h
   StatisticalMean.cc
   StatisticalMean.h
   SumFactory.cc
   SumFactory.h
)

add_library(statisticalmean STATIC ${statisticalmean_STAT_SRCS})
