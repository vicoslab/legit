/*******************************************************************************
* Copyright (c) 2013, Luka Cehovin
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of Ljubljana nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL LUKA CEHOVIN BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/
/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#include <math.h>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include "utils.h"
#include "debug.h"

namespace legit
{

namespace common
{

Rect get_roi(Mat& mat)
{
  Size s;
  Point o;
  Rect roi;

  mat.locateROI(s, o);

  roi.x = o.x;
  roi.y = o.y;
  roi.width = mat.cols;
  roi.height = mat.rows;

  return roi;
}

void printpoint(Point2f p)
{
  printf("Point (%.3f, %.3f) \n", p.x, p.y);
}

void printrect(Rect r)
{
  printf("Rectangle (%d, %d, w: %d, h: %d) \n", r.x, r.y, r.width, r.height);
}

void printsize(cv::Size s)
{
  printf("Size %d x %d \n", s.width, s.height);
}

void printsize(Mat& A)
{
  printf("Matrix size %d x %d \n", A.cols, A.rows);
}

void printmat(Mat& A)
{
  int i, j;
  int nc = A.cols;
  int nr = A.rows;

  switch (A.depth())
    {
    case CV_64F:
      printf("Double matrix: \n");
      for (i = 0; i < nr; i++)
        {
          printf("\n");
          for (j = 0; j < nc; j++)
            printf ("%.3f ", A.at<double>(i, j));
        }
      break;
    case CV_32F:
      printf("Float matrix: \n");
      for (i = 0; i < nr; i++)
        {
          printf("\n");
          for (j = 0; j < nc; j++)
            printf ("%.3f ", A.at<float>(i, j));
        }
      break;
    case CV_8U:
      printf("Byte matrix: \n");
      for (i = 0; i < nr; i++)
        {
          printf("\n");
          for (j = 0; j < nc; j++)
            printf ("%d ", A.at<uchar>(i, j));
        }
      break;

    }
  printf("\n");
}

#define LINE_BUFFER_SIZE (1024 * 3)

void read_csv(Mat& mat, const char* file, char delimiter)
{

  char buffer[LINE_BUFFER_SIZE];

  std::fstream in(file, fstream::in);

  vector<vector<float> > data;

  int cols = 0;

  while (true)
    {
      in.getline(buffer, LINE_BUFFER_SIZE);

      if (in.gcount() < 1)
        break;

      vector<float> row;

      row.push_back(atof(strtok(buffer, &delimiter)));

      while(true)
        {
          char* token = strtok(NULL, &delimiter);
          if (!token) break;
          row.push_back(atof(token));
        }

      cols = MAX(cols, row.size());
      data.push_back(row);
    }

  in.close();

  mat.create(data.size(), cols, CV_32F);

  for (size_t j = 0; j < data.size(); j++)
    {

      for (size_t i = 0; i < data[j].size(); i++)
        {
          mat.at<float>(j, i) = data[j][i];
        }
    }


}

void patch_operation(Mat& target, Mat& patch, Point position, int operation, int origin)
{

  Rect troi = get_roi(target);
  Rect proi = get_roi(patch);

  Rect troi2, proi2;

  switch (origin)
    {
    case ORIGIN_CENTER:
      position.x -= proi.width / 2;
      position.y -= proi.height / 2;
      break;
    case ORIGIN_BOTTOMRIGHT:
      position.x -= proi.width;
      position.y -= proi.height;
      break;
    }

  int x1 = MAX(troi.x, proi.x + position.x);
  int y1 = MAX(troi.y, proi.y + position.y);

  int x2 = MIN(troi.x + troi.width, proi.x + proi.width + position.x);
  int y2 = MIN(troi.y + troi.height, proi.y + proi.height + position.y);

  troi2.width = proi2.width = x2 - x1;
  troi2.height = proi2.height = y2 - y1;

  troi2.x = x1;
  troi2.y = y1;

  proi2.x = x1 - position.x;
  proi2.y = y1 - position.y;

  if (troi2.width < 1 || troi2.height < 1) return;

  switch (operation)
    {
    case OPERATION_SET:
    {
      Mat tt = target(troi2);
      patch(proi2).copyTo(tt);
      break;
    }
    case OPERATION_ADD:
      target(troi2) += patch(proi2);
      break;
    case OPERATION_SUBSTRACT:
      target(troi2) -= patch(proi2);
      break;
    case OPERATION_MULTIPLY:
      target(troi2) = target(troi2).mul(patch(proi2));
      break;
    default:
      throw LegitException("Unknown opeation");
    }

}

void patch_create(Mat& patch, int width, int height, int type, int flags)
{

  int cx = width / 2;
  int cy = height / 2;

  patch.create(width, height, CV_32F);

  for (int j = 0; j < height; j++)
    {
      for (int i = 0; i < width; i++)
        {
          float rx = 2.0f * (i - cx) / (float)width;
          float ry = 2.0f * (j - cy) / (float)height;
          float v = 0;
          switch (type)
            {
            case PATCH_DISC:
              v = 1 - (rx * rx + ry * ry);
              v = v < 0 ? 0.0f : 1.0f;
              break;
            case PATCH_CONE:
              v = 1 - (rx * rx + ry * ry);
              v = v < 0 ? 0.0f : v;
              break;
            case PATCH_GAUSS:
              v = (float)exp(-0.5 * (rx * rx * 3 + ry * ry * 3));
              break;
            }
          patch.at<float>(j, i) = (flags & FLAG_INVERT) ? 1 - v : v;
        }
    }

  if (flags & FLAG_NORMALIZE)
    patch /= sum(patch)[0];

}

cv::Rect get_mask_bounds(Mat& mat)
{

  int x1 = mat.cols, y1 = mat.rows, x2 = 0, y2 = 0;

  for (int j = 0; j < mat.rows; j++)
    {
      uchar* src = mat.ptr<uchar>(j);
      for (int i = 0; i < mat.cols; i++)
        if (src[i])
          {
            x1 = MIN(x1, i);
            y1 = MIN(y1, j);
            x2 = MAX(x2, i);
            y2 = MAX(y2, j);
          }
    }

  return cv::Rect(x1, y1, x2 - x1, y2 - y1);

}

const int opencv_mat_type_count = 28; // 7 base types, with five channel options each (none or C1, ..., C4)

const int opencv_mat_types[] =       {CV_8UC1,  CV_8UC2,  CV_8UC3,  CV_8UC4,
                                      CV_8SC1,  CV_8SC2,  CV_8SC3,  CV_8SC4,
                                      CV_16UC1, CV_16UC2, CV_16UC3, CV_16UC4,
                                      CV_16SC1, CV_16SC2, CV_16SC3, CV_16SC4,
                                      CV_32SC1, CV_32SC2, CV_32SC3, CV_32SC4,
                                      CV_32FC1, CV_32FC2, CV_32FC3, CV_32FC4,
                                      CV_64FC1, CV_64FC2, CV_64FC3, CV_64FC4
                                     };

const string opencv_mat_type_names[] = {"CV_8UC1",  "CV_8UC2",  "CV_8UC3",  "CV_8UC4",
                                        "CV_8SC1",  "CV_8SC2",  "CV_8SC3",  "CV_8SC4",
                                        "CV_16UC1", "CV_16UC2", "CV_16UC3", "CV_16UC4",
                                        "CV_16SC1", "CV_16SC2", "CV_16SC3", "CV_16SC4",
                                        "CV_32SC1", "CV_32SC2", "CV_32SC3", "CV_32SC4",
                                        "CV_32FC1", "CV_32FC2", "CV_32FC3", "CV_32FC4",
                                        "CV_64FC1", "CV_64FC2", "CV_64FC3", "CV_64FC4"
                                       };

const string describe_mat_type(int cvtype)
{


  for(int i=0; i<opencv_mat_type_count; i++)
    {
      if(cvtype == opencv_mat_types[i]) return opencv_mat_type_names[i];
    }

  return "<Unknown image type>";
}


LegitException::LegitException(std::string ss) : s(ss)
{
  DEBUGMSG("Throwing Legit exception: %s \n", ss.c_str());
}

const char* LegitException::what() const throw()
{
  return s.c_str();
}


}

}
