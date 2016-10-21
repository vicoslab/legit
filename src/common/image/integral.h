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

#ifndef _LEGIT_INTEGRAL_H
#define _LEGIT_INTEGRAL_H

#include <string.h>
#include <opencv2/core/core.hpp>

#ifdef PLATFORM_WINDOWS
#include "common/platform/windows/stdint_win.h"
#else
#include <stdint.h>
#endif

#include "common/utils/utils.h"
#include "common/image/histogram.h"

namespace legit
{

namespace common
{

class IntegralImage
{
public:
  IntegralImage(Mat& src);
  IntegralImage(Mat& src, float threshold);
  ~IntegralImage();

  void update(Mat& src);
  void update(Mat& src, float threshold);

  inline int sum(int x1, int y1, int x2, int y2)
  {

    return data[y2 * width + x2] - data[y1 * width + x2] - data[y2 * width + x1] + data[y1 * width + x1];

  }

  inline int get_width()
  {
    return width;
  };
  inline int get_height()
  {
    return height;
  };

  void print();

protected:
  int width;
  int height;

  uint32_t* data;
};


class IntegralHistogram
{
public:
  IntegralHistogram(Mat& src, int bins);
  ~IntegralHistogram();

  void update(Mat& src);

  inline SimpleHistogram sum(int x1, int y1, int x2, int y2)
  {

    SimpleHistogram hist = allocate_histogram(bins);

    sum(x1, y1, x2, y2, hist);

    return hist;

  }

  inline void sum(int x1, int y1, int x2, int y2, SimpleHistogram& hist)
  {

    DEBUGGING
    {
      assert(hist.size == bins);
    }

    uint32_t* dp = &(data[y2 * width + x2]);
    uint32_t* bp = &(data[y1 * width + x2]);
    uint32_t* cp = &(data[y2 * width + x1]);
    uint32_t* ap = &(data[y1 * width + x1]);

    hist.sum = 0;

    for (int b = 0; b < bins; b++)
      {
        hist.data[b] = dp[b] - cp[b] - bp[b] + ap[b];
        hist.sum += hist.data[b];
      }

  }

  inline void sum(cv::Point p, int half_size, SimpleHistogram& hist)
  {

    int x1 = MAX(p.x - half_size, 0);
    int y1 = MAX(p.y - half_size, 0);
    int x2 = MIN(p.x + half_size, width);
    int y2 = MIN(p.y + half_size, height);

    sum(x1, y1, x2, y2, hist);

  }

  inline int get_width()
  {
    return width;
  };
  inline int get_height()
  {
    return height;
  };
  inline int get_bins()
  {
    return bins;
  };

  void print(int bin);

private:

  int width;
  int height;
  int bins;
  int shift;

  uint32_t* data;

};

}

}

#endif