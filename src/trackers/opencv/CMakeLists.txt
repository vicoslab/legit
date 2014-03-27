CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

LEGIT_ADD_SOURCES(
  object_tracker.cpp
  cv_onlineboosting.cpp
  cv_onlinemil.cpp
  wrappers.cpp
)

LEGIT_REGISTER_TRACKER(wrappers.h OnlineBoostingTracker)
LEGIT_REGISTER_TRACKER(wrappers.h MILTracker)
