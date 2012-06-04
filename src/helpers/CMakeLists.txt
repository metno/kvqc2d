
include_directories(..)

########## helpers static library ##########

SET(helpers_STAT_SRCS
   Akima.cc
   Akima.h
   AkimaSpline.cc
   AkimaSpline.h
   AlgorithmHelpers.cc
   AlgorithmHelpers.h
   ConfigParser.cc
   ConfigParser.h
   FormulaUU.cc
   FormulaUU.h
   Helpers.cc
   Helpers.h
   WeightedMean.cc
   WeightedMean.h
   mathutil.cc
   mathutil.h
   StationParamParser.cc
   StationParamParser.h
   stringutil.cc
   stringutil.h
   timeutil.cc
   timeutil.h
)

add_library(helpers STATIC ${helpers_STAT_SRCS})
