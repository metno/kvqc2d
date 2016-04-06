
include_directories(.)

########## QC2 daemon ##########

SET(kvqc2d_SRCS
   Qc2Main.cc
)

add_executable(kvqc2d ${kvqc2d_SRCS})

target_link_libraries(kvqc2d kvqc2d_1 interpolation statisticalmean kvqc2d_2 helpers
    ${PC_KVCPP_LIBRARIES}
    ${Boost_THREAD_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

install(TARGETS kvqc2d DESTINATION bin)

########## two separate libs for correct linking, not nice but functional ##########

SET(kvqc2d_1_STAT_SRCS
   AlgorithmConfig.cc
   AlgorithmConfig.h
   AlgorithmDispatcher.cc
   AlgorithmDispatcher.h
   AlgorithmRunner.cc
   AlgorithmRunner.h
   Broadcaster.h
   algorithms/AggregatorLimits.cc
   algorithms/AggregatorLimits.h
   algorithms/DataUpdate.cc
   algorithms/DataUpdate.h
   algorithms/DipTestAlgorithm.cc
   algorithms/DipTestAlgorithm.h
   algorithms/GetStationParam.cc
   algorithms/GetStationParam.h
   algorithms/NeighborsDistance2.cc
   algorithms/NeighborsDistance2.h
   algorithms/ParseParValFile.cc
   algorithms/ParseParValFile.h
   algorithms/PlumaticAlgorithm.cc
   algorithms/PlumaticAlgorithm.h
   algorithms/RedistributionAlgorithm.cc
   algorithms/RedistributionAlgorithm.h
   algorithms/RedistributionNeighbors.cc
   algorithms/RedistributionNeighbors.h
   algorithms/SingleLinearAlgorithm.cc
   algorithms/SingleLinearAlgorithm.h
   DBInterface.h
   debug.h
   FlagChange.cc
   FlagChange.h
   FlagPattern.cc
   FlagPattern.h
   FlagPatterns.cc
   FlagPatterns.h
   FlagUpdate.cc
   FlagUpdate.h
   foreach.h
   InitLogger.cc
   InitLogger.h
   KvalobsDB.cc
   KvalobsDB.h
   KvalobsDbGate.cc
   KvalobsDbGate.h
   KvalobsElemExtract.h
   KvServicedBroadcaster.cc
   KvServicedBroadcaster.h
   LogfileNotifier.cc
   LogfileNotifier.h
   SingleFileLogStream.cc
   SingleFileLogStream.h
   SQLDataAccess.cc
   SQLDataAccess.h
   Qc2Algorithm.cc
   Qc2Algorithm.h
   Qc2App.cc
   Qc2App.h
)

add_library(kvqc2d_1 STATIC ${kvqc2d_1_STAT_SRCS})

SET(kvqc2d_2_STAT_SRCS
   Instrument.cc
   Instrument.h
   Notifier.cc
   Notifier.h
   Notifier.icc
   TimeRange.cc
   TimeRange.h
)

add_library(kvqc2d_2 STATIC ${kvqc2d_2_STAT_SRCS})


########## generate version.h from git commit ##########

# see http://www.cmake.org/pipermail/cmake/2010-July/038015.html

FILE(WRITE ${CMAKE_BINARY_DIR}/version.h.in
"\#define KVQC2D_VERSION_FULL \"@VERSION@\"\n"
)

FILE(WRITE ${CMAKE_BINARY_DIR}/version.cmake
"EXECUTE_PROCESS(
     COMMAND ${CMAKE_SOURCE_DIR}/version.sh
     OUTPUT_VARIABLE VERSION
     OUTPUT_STRIP_TRAILING_WHITESPACE
 )
 CONFIGURE_FILE(\${SRC} \${DST} @ONLY)
")

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

ADD_CUSTOM_TARGET(
    version
    ${CMAKE_COMMAND} -D SRC=${CMAKE_BINARY_DIR}/version.h.in
                     -D DST=${CMAKE_BINARY_DIR}/version.h
                     -P ${CMAKE_BINARY_DIR}/version.cmake
)

ADD_DEPENDENCIES(kvqc2d version)
