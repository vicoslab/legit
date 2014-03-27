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

#ifndef _LEGIT_MODALITIES_COLOR
#define _LEGIT_MODALITIES_COLOR

#include "modalities.h"

using namespace cv;
using namespace std;
using namespace legit::common;

namespace legit
{

namespace tracker
{

class ModalityColor3DHistogram : public Modality
{

public:
  ModalityColor3DHistogram(Config& config, string configbase);
  ~ModalityColor3DHistogram();

  virtual void flush();

  virtual void update(Image& image, PatchSet* patches, cv::Rect bounds);

  virtual bool usable();

  virtual void probability(Image& image, Mat& p);

private:
  int colorspace;
  int channels[3];
  int histSize[3];
  float ranges1[2];
  float ranges2[2];
  float ranges3[2];
  const float* ranges[3];
  bool has_data;
  float foreground_presistence;
  float background_presistence;
  int background_margin, background_size;
  float foreground_size;
  Mat tmp_p;
  MatND foreground;
  MatND background;
  MatND model;
};

}

}

#endif