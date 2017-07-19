project(testDyn)
set (CMAKE_CXX_STANDARD 11)

cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../..")

find_package(Boost COMPONENTS system filesystem signals thread REQUIRED)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} ) 

add_subdirectory(libopenframeworksarduino)

#include(openFrameworks)
include_directories(libopenframeworksarduino)

set(INCS
  src/stdafx.h
  src/ArduinoTest.h
  src/HexapodTimingTest.h
  src/PlaybackMovements.h
)

SET(SRCS
    src/ArduinoTest.cpp
    src/HexapodTimingTest.cpp
    src/PlaybackMovements.cpp
    src/Test.cpp	
)

add_executable(testDyn ${INCS} ${SRCS})
target_link_libraries(testDyn ${OPENFRAMEWORKS_LIBRARIES} ${Boost_LIBRARIES} openframeworksarduino)

