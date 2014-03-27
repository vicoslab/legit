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

#include "shape.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "../external/delaunay.h"

namespace legit
{

namespace tracker
{

/********************************************************************************
*
****                  CONVEX HULL SHAPE                                      ****
*
*********************************************************************************/

ModalityConvex::ModalityConvex(Config& config, string configbase) : Modality(config, configbase)
{
  margin = config.read<int>(configbase + ".margin", 10);
  margin_diminish = CLAMP3(config.read<float>(configbase + ".diminish", 0.3f), 0, 0.9999f);
  persistence = CLAMP3(config.read<float>(configbase + ".persistence", 0.5f), 0, 0.9999f);
  flush();
}

ModalityConvex::~ModalityConvex()
{

}

void ModalityConvex::flush()
{
  if (!history.empty())
    history.setTo(0);
}

void ModalityConvex::update(Image& image, PatchSet* patchSet, Rect bounds)
{

  Ptr<PatchSet> patches = reliablePatchesFilter.empty() ? patchSet : patchSet->filter(*reliablePatchesFilter);

  if (patches->size() < 3)
    {
      //flush();
      return;
    }

  Mat temp = image.get_float_mask();

  temp.setTo(0);

  if (history.empty())
    {
      history.create(image.height(), image.width(), CV_32F);
      history.setTo(0);
    }

  Point2f * points = new Point2f[patches->size()];
  Point2f * hull = NULL;

  Point2f offset = Point2f(image.width()/2, image.height() /2) - patchSet->mean_position();

  Point2f mean(0, 0);

  for (int i = 0; i < patches->size(); i++)
    {
      points[i] = patches->get_position(i) + offset;
    }

  int size = convex_hull(points, patches->size(), &hull);

  for (int i = 0; i < size; i++)
    {
      mean.x += hull[i].x;
      mean.y += hull[i].y;
    }

  mean.x /= size;
  mean.y /= size;

  Point * hulli = new Point[size];
  Point * hullei = new Point[size];

  for (int i = 0; i < size; i++)
    {
      hulli[i].x = (int) hull[i].x;
      hulli[i].y = (int) hull[i].y;
    }

  expand(hull, size, mean, margin);

  for (int i = 0; i < size; i++)
    {
      hullei[i].x = (int) hull[i].x;
      hullei[i].y = (int) hull[i].y;
    }

  fillConvexPoly(temp, hullei, size, Scalar(1 - margin_diminish));
  fillConvexPoly(temp, hulli, size, Scalar(1));

  delete [] points;
  free(hull);
  delete [] hulli;
  delete [] hullei;

  history = history * persistence + temp * (1.0f - persistence);

  debugCanvas->draw(history);
  debugCanvas->push();

}

bool ModalityConvex::usable()
{
  return !history.empty();
}

void ModalityConvex::probability(Image& image, Mat& p)
{

  Rect roi = image.get_roi();

  roi.x = (history.cols - roi.width) / 2;
  roi.y = (history.rows - roi.height) / 2;

  history(roi).copyTo(p);

  p /= sum(p)[0];

}

/********************************************************************************
*
****                  BOUNDING BOX REGION                                    ****
*
*********************************************************************************/


ModalityBounding::ModalityBounding(Config& config, string configbase) : Modality(config, configbase)
{
  margin = config.read<int>(configbase + ".expand", 0);
  bounds.width = -1;
  flush();
}

ModalityBounding::~ModalityBounding()
{

}

void ModalityBounding::flush()
{
  bounds.width = -1;
}

void ModalityBounding::update(Image& image, PatchSet* patchSet, Rect bounds)
{

  //Ptr<PatchSet> patches = reliablePatchesFilter.empty() ? patchSet : patchSet->filter(*reliablePatchesFilter);

  this->bounds = expand(bounds, margin);

}

bool ModalityBounding::usable()
{
  return bounds.width > 0;
}

void ModalityBounding::probability(Image& image, Mat& p)
{

  Rect roi = image.get_roi();

  Rect r = bounds;

  r.x -= roi.x;
  r.y -= roi.y;

  p.setTo(0);

  rectangle(p, r.tl(), r.br(), Scalar(1), FILLED);

  p /= sum(p)[0];

}

}

}