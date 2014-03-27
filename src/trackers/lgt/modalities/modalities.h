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

#ifndef LEGIT_MODALITIES
#define LEGIT_MODALITIES

#include <string.h>
#include <opencv2/core/core.hpp>
#include "../patches/patchset.h"
#include "common/utils/config.h"
#include "common/utils/utils.h"
#include "common/utils/defs.h"
#include "common/gui/gui.h"

using namespace cv;
using namespace std;
using namespace legit::common;

namespace legit
{

namespace tracker
{

class ReliablePatchesFilter : public Filter
{
public:
  ReliablePatchesFilter(float weight, int age) : weight(weight), age(age) {}
  ~ReliablePatchesFilter() {}
  virtual bool operator() (Ptr<Patch>& patch)
  {
    return (patch->get_weight(0) > weight && patch->get_age() > age);
  }
private:
  float weight;
  int age;
};

class Modality
{
public:
  Modality(Config& config, string configbase);

  virtual void flush() = 0;

  virtual void update(Image& image, PatchSet* patches, cv::Rect bounds) = 0;

  virtual bool usable() = 0;

  virtual void probability(Image& image, Mat& p) = 0;

  //virtual string get_name() = 0;

protected:

  Ptr<ReliablePatchesFilter> reliablePatchesFilter;

  Canvas* debugCanvas;

};

class Modalities
{

public:

  Modalities(Config& config);
  ~Modalities();

  void flush();

  void update(Image& image, PatchSet* patches, cv::Rect bounds);

  void probability(Image& image, Mat& p);

  void probability(Image& image, Mat& p, int i);

  int size();

protected:

  vector<Ptr<Modality> > modalities;

  Canvas* debugCanvas;

};

}

}

#endif