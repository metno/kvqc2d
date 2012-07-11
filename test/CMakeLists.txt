include_directories(${CMAKE_SOURCE_DIR}/src)

# include directory for generated files
include_directories(${CMAKE_CURRENT_BINARY_DIR}/algorithms)

find_package(Boost COMPONENTS regex REQUIRED)

find_package(GTest REQUIRED)
set(GTEST_DIR /usr/src/gtest)
add_subdirectory(${GTEST_DIR} ${CMAKE_CURRENT_BINARY_DIR}/gtest EXCLUDE_FROM_ALL)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/gtest)

find_library(SQLITE3_LIBRARY sqlite3)

########### next target ###############

ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/algorithms/StatisticalMean_n110.icc
   COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/algorithms &&
           python ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/build_daymeans_110.py ${CMAKE_SOURCE_DIR}/data/normals_110.dat ${CMAKE_CURRENT_BINARY_DIR}/algorithms/StatisticalMean_n110.icc
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/build_daymeans_110.py ${CMAKE_SOURCE_DIR}/data/normals_110.dat
)
ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/algorithms/StatisticalMean_n212.icc
   COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/build_daymeans_212.py ${CMAKE_SOURCE_DIR}/data/normals_212.dat ${CMAKE_CURRENT_BINARY_DIR}/algorithms/StatisticalMean_n212.icc
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/build_daymeans_212.py ${CMAKE_SOURCE_DIR}/data/normals_212.dat
)

SET(kvqc2dTest_SRCS
   AkimaTest.cc
   AlgorithmConfigTest.cc
   algorithms/AggregatorLimitsTest.cc
   algorithms/AkimaSplineTest.cc
   algorithms/AlgorithmHelpersTest.cc
   algorithms/AlgorithmTestBase.cc
   algorithms/AlgorithmTestBase.h
   algorithms/DataUpdateTest.cc
   algorithms/DipTestTest.cc
   algorithms/GapInterpolationTest.cc
   algorithms/PlumaticTest.cc
   algorithms/RedistributionTest.cc
   algorithms/SingleLinearTest.cc
   algorithms/TestBroadcaster.cc
   algorithms/TestBroadcaster.h
   algorithms/TestData.cc
   algorithms/TestData.h
   algorithms/TestDB.cc
   algorithms/TestDB.h
   algorithms/TestNotifier.cc
   algorithms/TestNotifier.h
   algorithms/StatisticalMeanTest.cc
   algorithms/StatisticalMean_n212.icc
   algorithms/StatisticalMean_n110.icc
   ConfigParserTest.cc
   FlagChangeTest.cc
   FlagPatternsTest.cc
   FlagPatternTest.cc
   FlagUpdateTest.cc
   FormulaUUTest.cc
   HelpersTest.cc
   test_main.cc
)

add_executable(kvqc2dTest ${kvqc2dTest_SRCS})

target_link_libraries(kvqc2dTest kvqc2d_1 interpolation statisticalmean kvqc2d_2 helpers
    ${PC_KVCPP_LIBRARIES}
    ${SQLITE3_LIBRARY}
    gtest
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

add_test(
    NAME unitTest
    COMMAND kvqc2dTest
)


########### corba test ###############

SET(corbatest_SRCS
   dummy-kvserviced.cc
)

add_executable(corbatest ${corbatest_SRCS})

target_link_libraries(corbatest
    ${PC_KVCPP_LIBRARIES}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

add_test(
    NAME corbaTest
    COMMAND corbatest
)
