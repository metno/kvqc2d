
include_directories(.. ../helpers)

########## interpolation algorithm static library ##########

SET(interpolation_STAT_SRCS
   GapData.cc
   GapData.hh
   GapUpdate.cc
   GapUpdate.hh
   GapInterpolationAlgorithm.cc
   GapInterpolationAlgorithm.h
   InterpolationData.cc
   InterpolationData.h
   InterpolationError.hh
   KvalobsMinMaxData.cc
   KvalobsMinMaxData.h
   KvalobsNeighborData.cc
   KvalobsNeighborData.h
   MinMaxInterpolator.cc
   MinMaxInterpolator.h
   MinMaxReconstruction.cc
   MinMaxReconstruction.h
   NeighborInterpolator.cc
   NeighborInterpolator.h
   ParameterInfo.cc
   ParameterInfo.h
   SingleParameterInterpolator.cc
   SingleParameterInterpolator.h
)

add_library(interpolation STATIC ${interpolation_STAT_SRCS})
