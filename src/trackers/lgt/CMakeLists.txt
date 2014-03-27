CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

LEGIT_ADD_SOURCES( 
	lgt.cpp 
	patches/patchset.cpp 
	patches/patch.cpp
	optimization/optimization.cpp 
	optimization/crossentropy.cpp 
	modalities/modalities.cpp 
	modalities/color.cpp 
	modalities/shape.cpp 
	modalities/motion.cpp
	external/delaunay.cpp
    external/triangle.c 
    external/chainhull.cpp
)

LEGIT_REGISTER_TRACKER(lgt.h LGTTracker)

LEGIT_REGISTER_CONFIGURATION(config/lgt.cfg)
