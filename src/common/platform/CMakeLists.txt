
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

LEGIT_ADD_SOURCES(filesystem.cpp)

IF(WIN32)
	LEGIT_ADD_SOURCES(windows/getopt_win.c windows/filesystem.cpp)
ENDIF(WIN32)

IF(UNIX)
	LEGIT_ADD_SOURCES(unix/filesystem.cpp)
ENDIF(UNIX)
