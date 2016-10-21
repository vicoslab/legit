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

#include "common/image/integral.h"

namespace legit
{

namespace common
{

IntegralImage::IntegralImage(Mat& src) : data(NULL)
{

  update(src);

}

IntegralImage::IntegralImage(Mat& src, float threshold) : data(NULL)
{

  update(src, threshold);

}


IntegralImage::~IntegralImage()
{

  if (data)
    {
      delete [] data;
    }

}

void IntegralImage::update(Mat& src)
{

  if (!data || (src.cols != width || src.rows != height))
    {
      if (data)
        delete [] data;

      width = src.cols;
      height = src.rows;

      data = new uint32_t[width * height];
    }

  memset(data, 0, width * height * sizeof(uint32_t));

  uchar* src_current = src.ptr<uchar>(0);
  uint32_t* dst_current = data;

  uint32_t row_sum = 0;
  for (int i = 0; i < width; i++)
    {
      dst_current[i] = row_sum;
      row_sum += src_current[i];
    }

  uint32_t* dst_previous = dst_current;
  dst_current += width;
  for (int j = 1; j < height; j++)
    {
      src_current = src.ptr<uchar>(j);

      uint32_t row_sum = 0;

      for (int i = 0; i < width; i++)
        {
          dst_current[i] = row_sum + dst_previous[i];
          row_sum += src_current[i];
        }

      dst_previous = dst_current;
      dst_current += width;
    }

}

void IntegralImage::update(Mat& src, float threshold)
{

  if (!data || (src.cols != width || src.rows != height))
    {
      if (data)
        delete [] data;

      width = src.cols;
      height = src.rows;

      data = new uint32_t[width * height];
    }

  memset(data, 0, width * height * sizeof(uint32_t));

  float* src_current = src.ptr<float>(0);
  uint32_t* dst_current = data;

  uint32_t row_sum = 0;
  for (int i = 0; i < width; i++)
    {
      dst_current[i] = row_sum;
      row_sum += (src_current[i] > threshold) ? 1 : 0;
    }

  uint32_t* dst_previous = dst_current;
  dst_current += width;
  for (int j = 1; j < height; j++)
    {
      src_current = src.ptr<float>(j);

      uint32_t row_sum = 0;

      for (int i = 0; i < width; i++)
        {
          dst_current[i] = row_sum + dst_previous[i];
          row_sum += (src_current[i] > threshold) ? 1 : 0;
        }

      dst_previous = dst_current;
      dst_current += width;
    }

}

void IntegralImage::print()
{

  printf("Integral image: \n");

  for (int j = 0; j < height; j++)
    {

      for (int i = 0; i < width; i++)
        {
          printf("%03d ", data[j * width + i]);
        }

      printf("\n");

    }

}

IntegralHistogram::IntegralHistogram(Mat& src, int bins) : bins(bins), data(NULL)
{


  switch(bins)
    {
    case 16:
      shift = 4;
      break;
    case 32:
      shift = 3;
      break;
    default:
      throw LegitException("Unsupported bins number");
    }

  update(src);

}

IntegralHistogram::~IntegralHistogram()
{

  if (data)
    {
      delete [] data;

    }
}

void IntegralHistogram::update(Mat& src)
{

  if (!data || (src.cols != width || src.rows != height))
    {
      if (data)
        delete [] data;

      width = src.cols;
      height = src.rows;

      data = new uint32_t[width * height * bins];

    }

  memset(data, 0, width * height * bins * sizeof(uint32_t));

  uchar* src_current = src.ptr<uchar>(0);
  uint32_t* dst_current = data;

  uint32_t* row_sum = new uint32_t[bins];
  memset(row_sum, 0, bins * sizeof(uint32_t));

  for (int i = 0; i < width; i++)
    {
      for (int b = 0; b < bins; b++)
        dst_current[b] = row_sum[b];
      row_sum[src_current[i] >> shift]++;
      dst_current += bins;
    }

  uint32_t* dst_previous = data;
  for (int j = 1; j < height; j++)
    {
      src_current = src.ptr<uchar>(j);

      memset(row_sum, 0, bins * sizeof(uint32_t));

      for (int i = 0; i < width; i++)
        {
          for (int b = 0; b < bins; b++)
            {
              dst_current[b] = row_sum[b] + dst_previous[b];
            }
          dst_current += bins;
          dst_previous += bins;
          row_sum[src_current[i] >> shift]++;
        }

    }

  delete [] row_sum;
}

void IntegralHistogram::print(int bin)
{

  printf("Integral histogram (bin %d): \n", bin);

  for (int j = 0; j < height; j++)
    {

      for (int i = 0; i < width; i++)
        {
          printf("%02d ", data[(j * width + i) * bins + bin]);
        }

      printf("\n");

    }

}

}

}

