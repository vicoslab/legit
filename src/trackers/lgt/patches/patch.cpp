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

#include <stdio.h>
#include "patch.h"

#define LAMBDA_VIS_H 0.063 //0.063  
#define LAMBDA_VIS_S 0.063 //0.063  
#define LAMBDA_VIS_V 0.063; //0.08 ; //8 ; //0.064
#define LAMBDA_VIS 3600.0 ;

namespace legit
{

namespace tracker
{

// -- Gray Histogram ---------------------------------------------------------------------- //
void HistogramPatch::initialize(Image& image, Point position)
{

  Mat grayscale = image.get_gray();

  histogram = calculate_histogram16(grayscale, position, width);

  temporary = allocate_histogram(16);

}

HistogramPatch::~HistogramPatch()
{

  release_histogram(histogram);

  release_histogram(temporary);

}

float HistogramPatch::response(Image& image, Point position)
{
  // 1-Bhattacharryya : 1 complete difference, 0 complete similarity
  Mat grayscale = image.get_gray();
  update_histogram16(grayscale, position, width >> 1, temporary);
  return (1.0-compare_histogram(temporary, histogram));


}

void HistogramPatch::responses(Image& image, Point2f* positions, int pcount, float* responses)
{
  // 1-Bhattacharryya : 1 complete difference, 0 complete similarity
  Mat grayscale = image.get_gray();
  int half_size = width >> 1;
  for (int i = 0; i < pcount; i++)
    {
      update_histogram16(grayscale, positions[i], half_size, temporary);
      responses[i] = (1.0-compare_histogram(temporary, histogram));
    }

}
// --RGB------------------------------------------------------------------------------- //
void RGBPatch::initialize(Image& image, Point position)
{
  // read current color at location "position"

  position.x = MAX(position.x, 0);
  position.y  = MAX(position.y, 0);
  position.x = MIN(position.x, image.width());
  position.y = MIN(position.y, image.height());

  Mat rgb = image.get_rgb() ; //src.depth() == CV_8U
  color.x = rgb.at<cv::Vec3b>(position)[0] ;
  color.y = rgb.at<cv::Vec3b>(position)[1] ;
  color.z = rgb.at<cv::Vec3b>(position)[2] ;

}

RGBPatch::~RGBPatch()
{

}

float RGBPatch::response(Image& image, Point position)
{
  // Mahalanobis distance, 0 complete similarity

  float dx, dy, dz ;

  // check if we are within image bounds
  if( position.x<0 || position.x>=image.width() || position.y<0 || position.y>=image.height())
    {
      return 255^2 * 3;
    }

  Mat rgb = image.get_rgb() ; //src.depth() == CV_8U
  dx = (rgb.at<cv::Vec3b>(position)[0]-color.x) ;
  dy = (rgb.at<cv::Vec3b>(position)[1]-color.y) ;
  dz = (rgb.at<cv::Vec3b>(position)[2]-color.z) ;
  dx = dx*dx ;
  dy = dy*dy ;
  dz = dz*dz ;
  return 0.5*(dx+dy+dz) / LAMBDA_VIS ;

}

void RGBPatch::responses(Image& image, Point2f* positions, int pcount, float* responses)
{
  // Mahalanobis distance, 0 complete similarity
  //Mat grayscale = image.get_gray();

  float dx, dy, dz ;

  Mat rgb = image.get_rgb() ;

  for (int i = 0; i < pcount; i++)
    {
      // check if we are within image bounds
      if( positions[i].x<0 || positions[i].x>=image.width() || positions[i].y<0 || positions[i].y>=image.height())
        {
          responses[i] = 195075.0 ; // 255^2 *3
        }
      else
        {

          dx = (rgb.at<cv::Vec3b>(positions[i])[0]-color.x) ;
          dy = (rgb.at<cv::Vec3b>(positions[i])[1]-color.y) ;
          dz = (rgb.at<cv::Vec3b>(positions[i])[2]-color.z) ;
          dx = dx*dx ;
          dy = dy*dy ;
          dz = dz*dz ;
          responses[i] = 0.5*(dx+dy+dz) / LAMBDA_VIS;

        }
    }
}

// --RGB------------------------------------------------------------------------------- //
void HSPatch::initialize(Image& image, Point position)
{
  // read current color at location "position"

  position.x = MAX(position.x, 0);
  position.y  = MAX(position.y, 0);
  position.x = MIN(position.x, image.width());
  position.y = MIN(position.y, image.height());

  Mat hsv = image.get(IMAGE_FORMAT_HSV);
  cv::Vec3b hsvcolor = hsv.at<cv::Vec3b>(position);
  color.x = ((float) hsvcolor[0]) / 255.0f;
  color.y = ((float) hsvcolor[1]) / 255.0f;
  color.z = ((float) hsvcolor[2]) / 255.0f;

}

HSPatch::~HSPatch()
{

}

float HSPatch::response(Image& image, Point position)
{
  // Mahalanobis distance, 0 complete similarity

  float dx, dy; //, dz ;

  // check if we are within image bounds
  if( position.x<0 || position.x>=image.width() || position.y<0 || position.y>=image.height())
    {
      return 255^2 *3;
    }

  Mat hsv = image.get(IMAGE_FORMAT_HSV);
  cv::Vec3b hsvcolor = hsv.at<cv::Vec3b>(position);
  dx = abs(((float) hsvcolor[0]) / 255.0f - color.x) ;
  dx = MIN(dx,1.0-dx);
  dy = ((float) hsvcolor[1]) / 255.0f - color.y ;
  //dz = hsv.at<cv::Vec3f>(position)[2] - color.z ;
  dx = dx * dx / LAMBDA_VIS_H ;
  dy = dy * dy / LAMBDA_VIS_S ;
  //dz = dz * dz / LAMBDA_VIS_V ;
  return 0.5 * (dx + dy); // + dz);


}

void HSPatch::responses(Image& image, Point2f* positions, int pcount, float* responses)
{
  // Mahalanobis distance, 0 complete similarity
  //Mat grayscale = image.get_gray();

  float dx, dy; //, dz ;

  Mat hsv = image.get(IMAGE_FORMAT_HSV);

  for (int i = 0; i < pcount; i++)
    {
      // check if we are within image bounds
      if( positions[i].x<0 || positions[i].x>=image.width() || positions[i].y<0 || positions[i].y>=image.height())
        {
          responses[i] = 255^2 *3;
        }
      else
        {
          cv::Vec3b hsvcolor = hsv.at<cv::Vec3b>(positions[i]);
          dx = abs((((float) hsvcolor[0]) / 255.0f) - color.x) ;
          dx = MIN(dx, 1.0 - dx) ;
          dy = ((float) hsvcolor[1]) / 255.0f - color.y ;
          //dz = hsv.at<cv::Vec3f>(positions[i])[2] - color.z ;
          dx = dx * dx / LAMBDA_VIS_H ;
          dy = dy * dy / LAMBDA_VIS_S ;
          //dz = dz * dz / LAMBDA_VIS_V ;
          responses[i] = 0.5 * (dx + dy); // + dz) ;
        }
    }
}


// -- SSD patch ------------------------------------------------------------------------------- //
void SSDPatch::initialize(Image& image, Point position)
{

  Mat gray = image.get_gray();

  tmpl.create(width, height, CV_8U);
  tmpl.setTo(0);
  patch_operation(tmpl, gray, -position + Point(width/2, height/2), OPERATION_SET, 0);

}

float SSDPatch::response(Image& image, Point position)
{

  Mat gray = image.get_gray();

  int x1 = MAX(position.x - width / 2, 0);
  int y1 = MAX(position.y - height / 2, 0);

  int x2 = MIN(position.x + width / 2, gray.cols);
  int y2 = MIN(position.y + height / 2, gray.rows);

  int ox = x1 - (position.x - width / 2);
  int oy = y1 - (position.y - height / 2);

  if (x1 >= x2 || y1 >= y2)
    return -50;

  float dist = 0;
  for (int j = 0; j < y2 - y1; j++)
    {
      uchar* data = & (gray.ptr<uchar>(j + y1)[x1]);
      uchar* tt = & (tmpl.ptr<uchar>(j + oy)[ox]);
      for (int i = 0; i < x2 - x1; i++)
        {
          int bin = data[i] - tt[i];
          dist += bin * bin;
        }
    }

//DEBUGMSG("%d %d %d %d %d %d %f %d\n", x1, x2, y1, y2, ox, oy, dist, c);

  return -(dist / ((x2 - x1) * (y2 - y1) * 255 * 255)) * 50;

}

void SSDPatch::responses(Image& image, Point2f* positions, int pcount, float* responses)
{

  for (int i = 0; i < pcount; i++)
    {
      responses[i] = response(image, positions[i]);
    }

}

}

}