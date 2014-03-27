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

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <opencv2/core/core.hpp>
#include "common/math/math.h"
#include "common/utils/utils.h"
#include "common/utils/debug.h"


using namespace std;
using namespace cv;

namespace legit
{

namespace common
{

typedef struct
{

  int32_t *data;
  int size;
  int sum;

} SimpleHistogram;

#define HIST_POW_8 3
#define HIST_SIZE_8 8

#define HIST_POW_16 4
#define HIST_SIZE_16 16

#define HIST_POW_32 5
#define HIST_SIZE_32 32

inline void update_histogram16(Mat& image, cv::Point position, int half_size, SimpleHistogram& histogram)
{

  /*    DEBUGGING {
          assert(histogram.size == HIST_SIZE_16);
      }*/

  int x1 = MAX(position.x - half_size, 0);
  int y1 = MAX(position.y - half_size, 0);
  int x2 = MIN(position.x + half_size, image.cols);
  int y2 = MIN(position.y + half_size, image.rows);

  memset(histogram.data, 0, sizeof(int32_t) * HIST_SIZE_16);
  for (int j = y1 ; j < y2; j++)
    {
      uchar* data = image.ptr<uchar>(j);
      for (int i = x1 ; i < x2; i++)
        {
          int bin = data[i] >> (8 - HIST_POW_16);
          histogram.data[bin]++;
        }
    }

  int N = (x2 - x1) * (y2 - y1);
  histogram.sum = MAX(N, 0);
}

inline SimpleHistogram calculate_histogram16(Mat& image, cv::Point position, int size)
{

  int size2 = size >> 1;
  SimpleHistogram histogram;
  int x1 = MAX(position.x - size2, 0);
  int y1 = MAX(position.y - size2, 0);
  int x2 = MIN(position.x + size2, image.cols);
  int y2 = MIN(position.y + size2, image.rows);

  histogram.data = new int32_t[HIST_SIZE_16];
  memset(histogram.data, 0, sizeof(int32_t) * HIST_SIZE_16);
  for (int j = y1 ; j < y2; j++)
    {
      uchar* data = image.ptr<uchar>(j);
      for (int i = x1 ; i < x2; i++)
        {
          int bin = data[i] >> (8 - HIST_POW_16);
          histogram.data[bin]++;
        }
    }

  int N = (x2 - x1) * (y2 - y1);
  histogram.sum = MAX(N, 0);
  histogram.size = HIST_SIZE_16;
  return histogram;
}


inline SimpleHistogram calculate_histogram32(Mat& image, cv::Point position, int size)
{

  int size2 = size >> 1;
  SimpleHistogram histogram;
  int x1 = MAX(position.x - size2, 0);
  int y1 = MAX(position.y - size2, 0);
  int x2 = MIN(position.x + size2, image.cols);
  int y2 = MIN(position.y + size2, image.rows);

  histogram.data = new int32_t[HIST_SIZE_32];
  memset(histogram.data, 0, sizeof(int32_t) * HIST_SIZE_32);
  for (int j = y1 ; j < y2; j++)
    {
      uchar* data = image.ptr<uchar>(j);
      for (int i = x1 ; i < x2; i++)
        {
          int bin = data[i] >> (8 - HIST_POW_32);
          histogram.data[bin]++;
        }
    }

  int N = (x2 - x1) * (y2 - y1);
  histogram.sum = MAX(N, 0);
  histogram.size = HIST_SIZE_32;
  return histogram;
}

inline float compare_histogram(SimpleHistogram& h1, SimpleHistogram& h2)
{
// Bhattacharryya coefficient: 0 complete difference, 1 complete similarity
  /*  if (!h1.sum || !h2.sum)
        return 1000;   // vrniti bi moral 0 ?
  	*/
  if (!h1.sum || !h2.sum)
    {
      if (h1.sum==0 && h2.sum==0)
        return 1.0 ;
      else
        return 0.0 ;
    }

  int32_t* hist1 = (int32_t*) h1.data;
  int32_t* hist2 = (int32_t*) h2.data;

  float bc = 0;

  for (int i = 0; i < h1.size; i++)
    {
      bc += SQRT((float)(hist1[i] * hist2[i]));
    }

  return bc * SQRT_INV((float)(h1.sum * h2.sum));
}

inline void release_histogram(SimpleHistogram& h)
{

  delete [] h.data;

}

inline SimpleHistogram allocate_histogram(int bins, bool reset = false)
{

  SimpleHistogram histogram;

  histogram.data = new int32_t[bins];

  if (reset)
    {
      histogram.sum = 0;
      memset(histogram.data, 0, sizeof(int32_t) * bins);
    }

  histogram.size = bins;

  return histogram;
}

inline void reset_histogram(SimpleHistogram& h)
{

  memset(h.data, 0, sizeof(int32_t) * h.size);
  h.sum = 0;

}

void print_histogram(SimpleHistogram h);



}

}


#endif