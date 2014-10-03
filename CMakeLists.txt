PROJECT (legit)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
GET_FILENAME_COMPONENT(PROJECT_ROOT . ABSOLUTE)
GET_FILENAME_COMPONENT(BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR} ABSOLUTE)
SET(CMAKE_MODULE_PATH ${PROJECT_ROOT}/cmake)
INCLUDE(LegitUtilities REQUIRED)
INCLUDE(PlatformUtilities REQUIRED)

ADD_DEFINITIONS(-DSOURCE_COMPILE_ROOT="${PROJECT_ROOT}/src/")

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

LEGIT_ADD_SOURCES(
	src/common/utils/utils.cpp
	src/common/utils/config.cpp
	src/common/utils/graphics.cpp
	src/common/utils/debug.cpp
	src/common/utils/string.cpp
	src/common/image/histogram.cpp
	src/common/image/sequence.cpp
	src/common/image/image.cpp
	src/common/image/integral.cpp
	src/common/math/statistics.cpp
	src/common/math/geometry.cpp
	src/common/gui/gui.cpp
	src/common/gui/window.cpp
	src/common/canvas.cpp
    src/api/legit.cpp
    src/observers.cpp
	src/tracker.cpp
)

ADD_SUBDIRECTORY(src/common/platform)

SET(BUILD_DEBUG TRUE CACHE BOOL "Enable debug support")

IF(BUILD_DEBUG)
	ADD_DEFINITIONS(-DBUILD_DEBUG)
	SET(ENABLE_DEBUG FALSE CACHE BOOL "Enable debug output by default")
	IF(ENABLE_DEBUG)
		ADD_DEFINITIONS(-DLEGIT_ENABLE_DEBUG)
	ENDIF(ENABLE_DEBUG)
ENDIF(BUILD_DEBUG)

SET(BUILD_FAST_MATH TRUE CACHE BOOL "Enable approximate math routines")

IF(BUILD_FAST_MATH)
	ADD_DEFINITIONS(-DBUILD_FAST_MATH)
	LEGIT_ADD_SOURCES(src/common/math/mersenne.cpp)
ENDIF(BUILD_FAST_MATH)

ADD_SUBDIRECTORY(src/trackers/)

LEGIT_GENERATE_HEADERS(${CMAKE_CURRENT_BINARY_DIR})
ADD_DEFINITIONS(-DBUILTIN_CONFIG="config.h")
ADD_DEFINITIONS(-DBUILTIN_TRACKERS="trackers.h")

set_property(SOURCE src/tracker.cpp APPEND PROPERTY OBJECT_DEPENDS "${BUILD_ROOT}/trackers.h" "${BUILD_ROOT}/config.h")


ADD_SUBDIRECTORY(src/api/)



ADD_LIBRARY(legit_objects OBJECT ${LEGIT_SOURCES})
ADD_LIBRARY(legit_static STATIC $<TARGET_OBJECTS:legit_objects>)
ADD_LIBRARY(legit SHARED $<TARGET_OBJECTS:legit_objects>)


SET(BUILD_TRAX FALSE CACHE BOOL "Enable TraX protocol support (requires libtrax)")

IF(BUILD_TRAX)
	SET(TRAX_DEBUG FALSE CACHE BOOL "Enable TraX debug")
	ADD_DEFINITIONS(-DBUILD_TRAX)
	TARGET_LINK_LIBRARIES(legit trax)
	IF(TRAX_DEBUG)
		ADD_DEFINITIONS(-DTRAX_DEBUG)
 	ENDIF(TRAX_DEBUG)
ENDIF(BUILD_TRAX)

ADD_EXECUTABLE(legit_runner src/runner.cpp)
TARGET_LINK_LIBRARIES(legit_runner legit)

INSTALL(TARGETS legit LIBRARY DESTINATION lib)
INSTALL(FILES src/api/legit.h DESTINATION include)
