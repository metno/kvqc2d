
include_directories(.. ../helpers)

########## interpolation algorithm static library ##########

SET(interpolation_STAT_SRCS
   InterpolationData.h
   SingleParameterInterpolator.cc
   SingleParameterInterpolator.h
   NeighborInterpolator.cc
   NeighborInterpolator.h
   SimpleInterpolationResult.h
   MinMaxInterpolator.cc
   MinMaxInterpolator.h
   KvalobsDataFilter.cc
   KvalobsDataFilter.h
   KvalobsNeighborData.cc
   KvalobsNeighborData.h
   KvalobsMinMaxData.cc
   KvalobsMinMaxData.h
   KvalobsDataUU.cc
   KvalobsDataUU.h
   ParameterInfo.cc
   ParameterInfo.h
   GapInterpolationAlgorithm.cc
   GapInterpolationAlgorithm.h
)

add_library(interpolation STATIC ${interpolation_STAT_SRCS})
