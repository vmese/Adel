# This CMakeLists.txt is intended to be used with ofnode CMake build system for openFrameworks
# see https://github.com/ofnode/of

project(arduinoOssiaController)
set(APP ${PROJECT_NAME})

cmake_minimum_required(VERSION 3.1)

set(OF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../of/" CACHE PATH "The root directory of ofnode/of project.")
include(${OF_ROOT}/openFrameworks.cmake)

ofxaddon(ofxOSSIA)
ofxaddon(ofxGui)
ofxaddon(ofxXmlSettings)

option(COTIRE "Use cotire" ON)

set(SOURCES
    src/main.cpp
    src/ofApp.cpp
    src/arduinoController.cpp
    src/arduinoController.h
    src/servo.cpp
    src/servo.h
)

add_executable(
    ${APP}
    MACOSX_BUNDLE
    ${SOURCES}
    ${OFXADDONS_SOURCES}
)

target_link_libraries(
    ${APP}
    ${OPENFRAMEWORKS_LIBRARIES}
    avahi-client
    avahi-common
)

set_target_properties(
    ${APP} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST
    "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set_target_properties( ${APP} PROPERTIES OUTPUT_NAME "${APP}-Debug")
endif()

if (CMAKE_CROSSCOMPILING)
    set_target_properties( ${APP} PROPERTIES OUTPUT_NAME
      "${APP}-${OF_PLATFORM}-${CMAKE_BUILD_TYPE}")
endif()

if (COTIRE)
    cotire(${APP})
endif()
