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

#ifndef LEGIT_CANVAS
#define LEGIT_CANVAS

#include "common/export.h"

#include <stdio.h>
#include <string.h>
#include <exception>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "common/math/geometry.h"

#define IMAGE_SCALE 1
#define IMAGE_STRETCH 3


using namespace cv;

namespace legit
{

namespace common
{

__LEGIT_EXPORT void drawcross(Mat& image, cv::Point center, Scalar color, int size = 3, bool sideways = false);

__LEGIT_EXPORT void drawellipse(Mat& image, cv::Point center, Matrix2f covariance, Scalar color, int width=1);

class __LEGIT_EXPORT Canvas
{
public:

  Canvas() {};

  ~Canvas() {};

  virtual void resize(cv::Size size) {};

  virtual void clear() {};

  virtual void draw(Mat& img, cv::Point offset=cv::Point(0, 0), int flags=0) {};

  virtual void rectangle(cv::Point tl, cv::Point br, Scalar color, int width=1) {};

  virtual void rectangle(cv::Rect r, Scalar color, int width=1) {};

  virtual void rectangle(RotatedRect r, Scalar color, int width=1) {};

  virtual void line(cv::Point p1, cv::Point p2, Scalar color, int width=1) {};

  virtual void cross(cv::Point p, Scalar color, int size=3) {};

  virtual void ellipse(cv::Point center, Matrix2f covariance, Scalar color, int width=1) {};

  virtual void circle(cv::Point center, int radius, Scalar color, int width=1) {};

  virtual void text(cv::Point p, string str, Scalar color) { };

  virtual void set_zoom(float z) { };

  virtual float get_zoom()
  {
    return 0;
  }

  virtual int width()
  {
    return 0;
  };

  virtual int height()
  {
    return 0;
  };

  virtual void push(int times = 1) {};

};

class __LEGIT_EXPORT ProxyCanvas : public Canvas
{
public:

  ProxyCanvas(Canvas* canvas, cv::Point offset=cv::Point(0, 0)) : Canvas(), canvas(canvas), offset(offset) {};

  ~ProxyCanvas() {};

  virtual void resize(cv::Size size);

  virtual void clear();

  virtual void draw(Mat& img, cv::Point offset=cv::Point(0, 0), int flags=0);

  virtual void rectangle(cv::Point tl, cv::Point br, Scalar color, int width=1);

  virtual void rectangle(cv::Rect r, Scalar color, int width=1);

  virtual void rectangle(RotatedRect r, Scalar color, int width=1);

  virtual void line(cv::Point p1, cv::Point p2, Scalar color, int width=1);

  virtual void cross(cv::Point p, Scalar color, int size=3);

  virtual void ellipse(cv::Point center, Matrix2f covariance, Scalar color, int width=1);

  virtual void text(cv::Point p, string str, Scalar color);

  virtual void circle(cv::Point center, int radius, Scalar color, int width=1);

  virtual void set_zoom(float z);

  virtual float get_zoom();

  virtual int width();

  virtual int height();

  virtual void push(int times = 1);

  void set_offset(cv::Point offset)
  {
    this->offset = offset;
  }

  cv::Point get_offset()
  {
    return offset;
  }

private:

  Canvas* canvas;

  cv::Point offset;

};


class __LEGIT_EXPORT ImageCanvas : public Canvas
{
public:

  ImageCanvas(cv::Size size);

  ImageCanvas(Mat& image);

  ~ImageCanvas();

  virtual void resize(cv::Size size);

  virtual void clear();

  virtual void draw(Mat& img, cv::Point offset=cv::Point(0, 0), int flags=0);

  virtual void rectangle(cv::Point tl, cv::Point br, Scalar color, int width=1);

  virtual void rectangle(cv::Rect r, Scalar color, int width=1);

  virtual void rectangle(RotatedRect r, Scalar color, int width=1);

  virtual void line(cv::Point p1, cv::Point p2, Scalar color, int width=1);

  virtual void cross(cv::Point p, Scalar color, int size=3);

  virtual void ellipse(cv::Point center, Matrix2f covariance, Scalar color, int width=1);

  virtual void text(cv::Point p, string str, Scalar color);

  virtual void circle(cv::Point center, int radius, Scalar color, int width=1);

  virtual void set_zoom(float z)
  {
    zoom = MIN(5, MAX(0.5f, z));
  };

  virtual float get_zoom()
  {
    return zoom;
  }

  virtual int width();

  virtual int height();

protected:

  Mat image;

  float zoom;
};

class __LEGIT_EXPORT VideoCanvas : public ImageCanvas
{
public:

  VideoCanvas(string filename, int fps, cv::Size size, bool isColor);

  ~VideoCanvas();

  virtual void resize(cv::Size size) {};

  virtual void push(int times = 1);

protected:

  Ptr<VideoWriter> writer;
};

__LEGIT_EXPORT void canvas_draw_transformed_rectangle(Canvas& canvas, cv::Rect rectangle, Matrix3f transform, Scalar color, int width=1);


}

}

#endif
