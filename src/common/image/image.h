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

#ifndef _LEGIT_IMAGE_H
#define _LEGIT_IMAGE_H

#include "common/export.h"
#include "common/utils/utils.h"
#include "common/image/sequence.h"
#include "common/image/integral.h"

#define IMAGE_FORMAT_GRAY 0
#define IMAGE_FORMAT_RGB 1
#define IMAGE_FORMAT_HSV 2
#define IMAGE_FORMAT_YCRCB 3
#define IMAGE_FORMATS 4

namespace legit
{

namespace common
{

class __LEGIT_EXPORT Image
{
public:
  Image();

  Image(int width, int height);

  Image(const std::string& path);

  Image(Mat& image);

  Image(Image& image, cv::Rect region);

  ~Image();

  void capture(Sequence* capture);

  void load(const std::string& path);

  void save(const std::string& path);

  void update(Mat& image);

  void update(Mat& data, int format, bool overwrite = true);

  void reset();

  void copy_region(Image& image, cv::Rect region);

  Mat get(int format);

  Mat get_rgb();

  Mat get_gray();

  Mat get_hsv();

  Mat get_mask();

  Mat get_float_mask();

  inline bool empty()
  {
    return _width == 0 && _height == 0;
  }

  inline int width()
  {
    return _width;
  };

  inline int height()
  {
    return _height;
  };

  Point2i get_offset();

  cv::Rect get_roi();

private:

  void update_size();

  int _width;
  int _height;

  bool has_format[IMAGE_FORMATS];
  Mat formats[IMAGE_FORMATS];

  bool has_mask;
  Mat mask;

  bool has_float_mask;
  Mat float_mask;

  bool has_inthist16;
  IntegralHistogram* inthist16;

  bool has_inthist32;
  IntegralHistogram* inthist32;

  bool has_integral_image;
  IntegralImage* integral_image;

  Point2i offset;
};

}

}

#endif
