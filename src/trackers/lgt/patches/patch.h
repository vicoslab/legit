/********************************************************************
* LGT tracker - The official C++ implementation of the LGT tracker
* Copyright (C) 2013  Luka Cehovin
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
********************************************************************/
/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef LEGIT_PATCH
#define LEGIT_PATCH

#include <opencv2/core/core.hpp>
#include "patchset.h"

using namespace cv;
using namespace std;
using namespace legit::common;

namespace legit
{

namespace tracker
{

class HistogramPatch : public Patch
{
public:
  HistogramPatch(int id, int capacity, int limit, int width, int height) : Patch(id, capacity, limit, width, height) {}
  ~HistogramPatch();

  virtual void initialize(Image& image, cv::Point position);
  virtual float response(Image& image, cv::Point position);
  virtual void responses(Image& image, cv::Point2f* positions, int pcount, float* responses);
  virtual PatchType get_type()
  {
    return HISTOGRAM;
  }

private:

  SimpleHistogram histogram;

  SimpleHistogram temporary;

  Point3f color; // hack
};


class RGBPatch : public Patch
{
public:
  RGBPatch(int id, int capacity, int limit) : Patch(id, capacity, limit, 1, 1) {}
  ~RGBPatch();

  virtual void initialize(Image& image, cv::Point position);
  virtual float response(Image& image, cv::Point position);
  virtual void responses(Image& image, cv::Point2f* positions, int pcount, float* responses);
  virtual PatchType get_type()
  {
    return RGBPIXEL;
  }
private:

  Point3f color;
};

class HSPatch : public Patch
{
public:
  HSPatch(int id, int capacity, int limit) : Patch(id, capacity, limit, 1, 1) {}
  ~HSPatch();

  virtual void initialize(Image& image, cv::Point position);
  virtual float response(Image& image, cv::Point position);
  virtual void responses(Image& image, cv::Point2f* positions, int pcount, float* responses);
  virtual PatchType get_type()
  {
    return HSPIXEL;
  }

private:

  Point3f color;
};

class SSDPatch : public Patch
{
public:
  SSDPatch(int id, int capacity, int limit, int width, int height) : Patch(id, capacity, limit, width, height) {}
  ~SSDPatch() {}

  virtual void initialize(Image& image, cv::Point position);
  virtual float response(Image& image, cv::Point position);
  virtual void responses(Image& image, Point2f* positions, int pcount, float* responses);

  virtual PatchType get_type()
  {
    return SSD;
  }

private:
  Mat tmpl;
};

}

}

#endif