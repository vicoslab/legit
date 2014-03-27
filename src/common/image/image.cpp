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

#include "common/image/image.h"
#include <opencv2/imgproc/imgproc.hpp>

namespace legit
{

namespace common
{

Image::Image() : offset(0, 0), integral_image(NULL), inthist16(NULL), inthist32(NULL)
{

  reset();

  update_size();

}

Image::Image(int width, int height) : offset(0, 0), integral_image(NULL), inthist16(NULL), inthist32(NULL)
{

  reset();

  formats[IMAGE_FORMAT_RGB].create(height, width, CV_8UC3);
  has_format[IMAGE_FORMAT_RGB] = true;

  update_size();

}

Image::Image(const std::string& path) : offset(0, 0), integral_image(NULL), inthist16(NULL), inthist32(NULL)
{

  load(path);

}

Image::Image(Mat& src) : offset(0, 0), integral_image(NULL), inthist16(NULL), inthist32(NULL)
{

  update(src);

}

// TODO: improve !!!
Image::Image(Image& image, Rect region) : offset(0, 0), integral_image(NULL), inthist16(NULL), inthist32(NULL)
{

  copy_region(image, region);

}

Image::~Image()
{

  reset();

}

void Image::capture(Sequence* capture)
{

  reset();

  capture->read_frame(formats[IMAGE_FORMAT_RGB]);

  if (formats[IMAGE_FORMAT_RGB].data)
    {
      has_format[IMAGE_FORMAT_RGB] = true;
    }

  offset = Point2i(0, 0);

  update_size();
}

void Image::load(const std::string& path)
{

  reset();

  Mat m = imread(path);

  if (m.channels() == 1)
    {
      update(m, IMAGE_FORMAT_GRAY);
    }
  else if (m.channels() == 3)
    {
      update(m, IMAGE_FORMAT_RGB);
    }

  update_size();
}

void Image::save(const std::string& path)
{

  if (empty())
    return;

  Mat rgb = get_gray();

#ifdef IOS

  throw LegitException("Saving image on IOS currently not supported");
  //cvSaveImage( path.c_str(), rgb, NULL);

#else

  vector<int> params;

  imwrite(path, rgb, params);

#endif

}

void Image::update(Mat& src)
{

  offset = Point(0, 0);

  if (src.channels() == 3 )
    {

      if (src.depth() == CV_8U)
        {

          update(src, IMAGE_FORMAT_RGB);

        }
      else throw LegitException("Unrecognized source type");
    }
  else if (src.channels() == 1 && src.depth() == CV_8U)
    {

      update(src, IMAGE_FORMAT_GRAY);

    }
  else throw LegitException("Unrecognized source type");

}

void Image::update(Mat& data, int format, bool overwrite)
{

  if (!overwrite && !empty() && (data.cols != _width || data.rows != _height))
    throw LegitException("Size of the input matrix does not match the size of the image");

  if (format < 0 || format >= IMAGE_FORMATS)
    throw LegitException("Unknown image format");

  if (overwrite) reset();

  data.copyTo(formats[format]);
  has_format[format] = true;

  update_size();
}

void Image::copy_region(Image& image, cv::Rect region)
{

  reset();

  Rect roi;

  roi.x = MIN(MAX(region.x, 0), image.width());
  roi.y = MIN(MAX(region.y, 0), image.height());
  roi.width = MIN(MAX(region.width + roi.x, 0), image.width()) - roi.x;
  roi.height = MIN(MAX(region.height + roi.y, 0), image.height()) - roi.y;

  for (int i = 0; i < IMAGE_FORMATS; i++)
    {
      if (!image.has_format[i]) continue;

      formats[i] = image.formats[i](roi);
      has_format[i] = true;
    }

  offset = roi.tl();

  update_size();
}

Mat Image::get_rgb()
{

  return get(IMAGE_FORMAT_RGB);

}

Mat Image::get_gray()
{

  return get(IMAGE_FORMAT_GRAY);

}

Mat Image::get_hsv()
{

  return get(IMAGE_FORMAT_HSV);

}

Mat Image::get(int format)
{

  if (format < 0 || format >= IMAGE_FORMATS)
    throw LegitException("Unknown image format");

  if (has_format[format])
    return formats[format];

  switch (format)
    {
    case IMAGE_FORMAT_GRAY:
    {
      Mat tmp_rgb = this->get(IMAGE_FORMAT_RGB);
      cvtColor(tmp_rgb, formats[IMAGE_FORMAT_GRAY], COLOR_RGB2GRAY);
      has_format[IMAGE_FORMAT_GRAY] = true;
      // TODO: do this smart for YCrCb ... just use Y
      break;
    }
    case IMAGE_FORMAT_RGB:
    {
      if (has_format[IMAGE_FORMAT_HSV])
        {
          cvtColor(formats[IMAGE_FORMAT_HSV], formats[IMAGE_FORMAT_RGB], COLOR_HSV2RGB);
          has_format[IMAGE_FORMAT_RGB] = true;
        }
      else if (has_format[IMAGE_FORMAT_YCRCB])
        {
          cvtColor(formats[IMAGE_FORMAT_YCRCB], formats[IMAGE_FORMAT_RGB], COLOR_YCrCb2RGB);
          has_format[IMAGE_FORMAT_RGB] = true;
        }
      else if (has_format[IMAGE_FORMAT_GRAY])
        {
          cvtColor(formats[IMAGE_FORMAT_GRAY], formats[IMAGE_FORMAT_RGB], COLOR_GRAY2RGB);
          has_format[IMAGE_FORMAT_RGB] = true;
        }
      else throw LegitException("Unable to return RGB image");
      break;
    }
    case IMAGE_FORMAT_HSV:
    {
      Mat tmp_rgb = get(IMAGE_FORMAT_RGB);
      cvtColor(tmp_rgb, formats[IMAGE_FORMAT_HSV], COLOR_RGB2HSV);
      has_format[IMAGE_FORMAT_HSV] = true;
      break;
    }
    case IMAGE_FORMAT_YCRCB:
    {
      Mat tmp_rgb = get(IMAGE_FORMAT_RGB);
      cvtColor(tmp_rgb, formats[IMAGE_FORMAT_YCRCB], COLOR_RGB2YCrCb);
      has_format[IMAGE_FORMAT_YCRCB] = true;
      break;
    }
    }

  return formats[format];

}

Point2i Image::get_offset()
{
  return offset;
}

Rect Image::get_roi()
{

  for (int i = 0; i < IMAGE_FORMATS; i++)
    {
      if (!has_format[i]) continue;
      return legit::common::get_roi(formats[i]);
    }

  throw LegitException("Empty image");
}

void Image::update_size()
{

  _width = 0;
  _height = 0;

  for (int i = 0; i < IMAGE_FORMATS; i++)
    {
      if (!has_format[i]) continue;

      _width = formats[i].cols;
      _height = formats[i].rows;
      break;
    }

}

void Image::reset()
{

  if (has_integral_image && integral_image)
    {
      delete integral_image;
      integral_image = NULL;
    }

  if (has_inthist16 && inthist16)
    {
      delete inthist16;
      inthist16 = NULL;
    }

  if (has_inthist32 && inthist32)
    {
      delete inthist32;
      inthist32 = NULL;
    }

  for (int i = 0; i < IMAGE_FORMATS; i++)
    has_format[i] = false;

  has_inthist16 = false;
  has_inthist32 = false;
  has_integral_image = false;
  has_mask = false;
  has_float_mask = false;

}

Mat Image::get_mask()
{

  if (!has_mask)
    {
      mask.create(height(), width(), CV_8U);
      has_mask = true;
    }

  return mask;

}


Mat Image::get_float_mask()
{

  if (!has_float_mask)
    {
      float_mask.create(height(), width(), CV_32F);
      has_float_mask = true;
    }

  return float_mask;

}

}

}
