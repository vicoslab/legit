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

#include "color.h"
#include "../external/delaunay.h"
#include <opencv2/imgproc/imgproc.hpp>

namespace legit
{

namespace tracker
{

void fillconvex_ring(Mat& mask, Point2f* points, int count, int inner, int outer)
{

  Point2f* hull;
  int size = convex_hull(points, count, &hull);

  // apparently we have to convert Point2f array to integers ...
  Point * hulli = new Point[size];
  for (int i = 0; i < size; i++)
    {
      hulli[i].x = (int) hull[i].x;
      hulli[i].y = (int) hull[i].y;
    }

  free(hull);

  fillConvexPoly(mask, hulli, size, Scalar(1), 1);

  delete [] hulli;

  Mat mask2;

  dilate(mask, mask2, Mat::ones(outer + inner, outer + inner, CV_8U));

  fillConvexPoly(mask, hulli, size, Scalar(0), 1);

  erode(mask2, mask, Mat::ones(inner, inner, CV_8U));

}


/**
Basic HSV FB-BG color histogram modality that uses OpenCV histograms.


*/
ModalityColor3DHistogram::ModalityColor3DHistogram(Config& config, string configbase) : Modality(config, configbase), has_data(false)
{

  channels[0] = 0;
  channels[1] = 1;
  channels[2] = 2;
  histSize[0] = config.read<int>(configbase + ".bins.first", 8);
  histSize[1] = config.read<int>(configbase + ".bins.second", 8);
  histSize[2] = config.read<int>(configbase + ".bins.third", 8);

  string cspace = config.read<string>(configbase + ".colorspace", "hsv");

  if (cspace == "hsv")
    {
      colorspace = IMAGE_FORMAT_HSV;
      ranges1[0] = 0;
      ranges1[1] = 180; // 180.0f / 256.0f;
      ranges2[0] = 0;
      ranges2[1] = 256; //1.0f;
      ranges3[0] = 0;
      ranges3[1] = 256; // 1.0f;
    }
  else if (cspace == "rgb")
    {
      colorspace = IMAGE_FORMAT_RGB;
      ranges1[0] = 0;
      ranges1[1] = 256; //1.0f;
      ranges2[0] = 0;
      ranges2[1] = 256; // 1.0f;
      ranges3[0] = 0;
      ranges3[1] = 256; //1.0f;
    }
  else if (cspace == "ycrcb")
    {
      colorspace = IMAGE_FORMAT_YCRCB;
      ranges1[0] = 0;
      ranges1[1] = 256; //1.0f;
      ranges2[0] = 0;
      ranges2[1] = 256; // 1.0f;
      ranges3[0] = 0;
      ranges3[1] = 256; //1.0f;
    }
  else throw LegitException("Unrecognized color space");

  ranges[0] = ranges1;
  ranges[1] = ranges2;
  ranges[2] = ranges3;

  foreground.create(3, histSize, CV_32F);
  background.create(3, histSize, CV_32F);
  model.create(3, histSize, CV_32F);

  foreground_presistence = config.read<int>(configbase + ".persistence.foreground");
  background_presistence = config.read<int>(configbase + ".persistence.background");

  foreground_size = config.read<double>(configbase + ".region.foreground");
  background_margin = config.read<int>(configbase + ".region.margin");
  background_size = config.read<int>(configbase + ".region.background");

  debugCanvas = get_canvas(config.read<string>(configbase + ".debug", ""));

  flush();
}

ModalityColor3DHistogram::~ModalityColor3DHistogram()
{

}

void ModalityColor3DHistogram::flush()
{
  float histCount = histSize[0] * histSize[1] * histSize[2];
  foreground.setTo(0);
  background.setTo(1 / histCount);
  model.setTo(0);
  has_data = false;
}

void ModalityColor3DHistogram::update(Image& image, PatchSet* patchSet, Rect bounds)
{

  Ptr<PatchSet> patches = reliablePatchesFilter.empty() ? patchSet : patchSet->filter(*reliablePatchesFilter);

  Rect background_outer = expand(bounds, background_size + background_margin);
  Rect background_inner = expand(bounds, background_margin);

  MatND new_foreground, new_background;

  //tmp_float.convertTo(image.get(colorspace), CV_32F, 1.0/255.0);
  Mat arrays[] = {image.get(colorspace)};

  Mat mask = image.get_mask();
  mask.setTo(0);

  int half_size = patches->get_radius() * foreground_size;

// Foreground histogram
  for (int i = 0; i < patches->size(); i++)
    {
      Point2f pos = patches->get_position(i);

      Rect r;
      r.x = CLAMP3( ((int)pos.x - half_size), 0, mask.cols);
      r.y = CLAMP3( ((int)pos.y - half_size), 0, mask.rows);
      r.width = CLAMP3( ((int)pos.x + half_size), 0, mask.cols) - r.x;
      r.height = CLAMP3( ((int)pos.y + half_size), 0, mask.rows) - r.y;

      if (r.width < 1 || r.height < 1) continue;

      mask(r) = 1;
    }

  calcHist(arrays, 1, channels, mask, new_foreground, 3, histSize, ranges, true, false);

  if (debugCanvas->get_zoom() > 0)
    {
      Mat masked;
      image.get_gray().copyTo(masked);
      masked = masked.mul(mask);
      debugCanvas->draw(masked, Point(0,0), IMAGE_STRETCH);
    }

  calcHist(arrays, 1, channels, mask, new_foreground, 3, histSize, ranges, true, false);


  mask.setTo(0);

  /*Point2f* convex_points = new Point2f[patches.size()];

  for (int i = 0; i < patches.size(); i++) {
      convex_points[i] = patches.get_position(i);
  }

  fillconvex(mask, convex_points, convex_points.size(), Scalar(1));*/

  Rect outer = expand(bounds, background_size + background_margin);
  rectangle(mask, outer.tl(), outer.br(), Scalar(1), FILLED);
  Rect inner = expand(bounds, background_margin);
  rectangle(mask, inner.tl(), inner.br(), Scalar(0), FILLED);


  rectangle(mask, background_outer.tl(), background_outer.br(), Scalar(1), FILLED);

  rectangle(mask, background_inner.tl(), background_inner.br(), Scalar(0), FILLED);

  calcHist(arrays, 1, channels, mask, new_background, 3, histSize, ranges, true, false);

  new_background += 1;

  if (debugCanvas->get_zoom() > 0)
    {
      Mat masked;
      image.get_gray().copyTo(masked);
      masked = masked.mul(mask);
      debugCanvas->draw(masked, Point(0,0), IMAGE_STRETCH);
    }

// Merging model with new data

  float* ofd = (float *) foreground.data;
  float* nfd = (float *) new_foreground.data;
  float* obd = (float *) background.data;
  float* nbd = (float *) new_background.data;

  float* md = (float *) model.data;

  float apriori = (float)(bounds.width * bounds.height) / (float)(image.width() * image.height()); // TODO: justify factor

  int histCount = histSize[0] * histSize[1] * histSize[2];

  float nfdSum = 0, nbdSum = 0;

  for (int i = 0; i < histCount; i++)
    {
      nfdSum += nfd[i];
      nbdSum += nbd[i];
    }

  float nfdSum2 = 0, nbdSum2 = 0;

  if (nfdSum > 0)
    {
      for (int i = 0; i < histCount; i++)
        {
          ofd[i] = foreground_presistence * ofd[i] + (1 - foreground_presistence) * (nfd[i] / nfdSum);
          nfdSum2 += ofd[i];
        }
    }
  else
    {
      for (int i = 0; i < histCount; i++)
        nfdSum2 += ofd[i];
    }

  if (nbdSum > 0)
    {
      for (int i = 0; i < histCount; i++)
        {
          obd[i] = background_presistence * obd[i] + (1 - background_presistence) * (nbd[i] / nbdSum);
          nbdSum2 += obd[i];
        }
    }
  else
    {
      for (int i = 0; i < histCount; i++)
        nbdSum2 += obd[i];
    }

  for (int i = 0; i < histCount; i++)
    md[i] = ((apriori * (ofd[i] / nfdSum2)) / (apriori * (ofd[i] / nfdSum2) + (1 - apriori) * (obd[i] / nbdSum2))) * 255;

  has_data = true;
}

bool ModalityColor3DHistogram::usable()
{
  return has_data;
}

void ModalityColor3DHistogram::probability(Image& image, Mat& p)
{

  Mat arrays[] = {image.get(colorspace)};

  calcBackProject(arrays, 1, channels, model, tmp_p, ranges, 1, true);

  tmp_p.convertTo(p, CV_32F, 1.0/sum(tmp_p)[0]);

}

}

}