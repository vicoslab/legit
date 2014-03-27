Legit - A visual tracking library
=================================

Legit is a C++ library that contains implementations of various visual tracking
algorithms. The main goal of the library is to have multiple different visual
trackers available in one place to be used for research in visual tracking as 
well as employing visual tracking in various tasks.

History
-------

The development of the library started in ViCoS lab in 2010 with a native C++
implementation of the LGT tracking algorithm, published at ICCV 2011. Because 
a lot of the code became quite general, the goal of the library became bigger 
and more ambitious with time. A public version of the library became available
in October 2013.

Building
========

At the moment the only dependency of the library is OpenCV computer vision 
library (version 2.1 or higher). The code can be compiled using CMake 
build system. Keep in mind that most tracker implementations are disabled
by default so the compilation process has to be configured first.

The library should compile on Windows, Linux and OSX platforms.

Legal
=====

The core of the library is under BSD license. Tracker implementations are 
licensed under different licenses.
