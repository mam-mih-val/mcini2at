cmake_minimum_required(VERSION 3.15)
project(mcini2at)

set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_PREFIX_PATH $ENV{McIniData_DIR})

find_package(ROOT REQUIRED COMPONENTS RIO Tree)

include(FetchContent)

FetchContent_Declare(
        AnalysisTree
        GIT_REPOSITORY https://github.com/HeavyIonAnalysis/AnalysisTree
        GIT_TAG        v1.0.9
)

FetchContent_MakeAvailable(AnalysisTree)


set(CMAKE_CXX_FLAGS "-Wall")
include(${ROOT_USE_FILE})

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${ROOT_INCLUDE_DIRS}
        $ENV{McIniData_DIR}/include
        $ENV{AnalysisTree_INCLUDE_DIR}/include
)

link_directories(
        ${CMAKE_SOURCE_DIR}
        ${AnalysisTree_LIBRARY_DIR}
)

add_executable(convert src/convert.cc src/converter.h src/converter.cc)
target_link_libraries(convert ${ROOT_LIBRARIES} ROOT::EG AnalysisTreeBase AnalysisTreeInfra $ENV{McIniData_DIR}/build/libMcIniData.so)

add_executable(multiplicity src/calculate.cc src/multiplicity.cc)
target_link_libraries(multiplicity ${ROOT_LIBRARIES} ROOT::EG $ENV{McIniData_DIR}/build/libMcIniData.so)

add_executable(cut_equal_bins src/cut_equal_bins.cc)
target_link_libraries(cut_equal_bins ${ROOT_LIBRARIES})