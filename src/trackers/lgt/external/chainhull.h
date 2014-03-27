/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef CHAINHULL_H
#define CHAINHULL_H

#include <opencv2/core/core.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;

int chainhull(Point2f* P, int n, Point2f* H);

#endif
