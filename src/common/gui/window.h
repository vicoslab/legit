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

#ifndef LEGIT_WINDOW
#define LEGIT_WINDOW

#include <stdio.h>
#include <string.h>
#include <exception>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "common/canvas.h"

namespace legit
{

namespace common
{


class ImageWindow : public ImageCanvas
{
public:
  static ImageWindow* create(string t, cv::Size size, bool aup = true);

  ~ImageWindow();

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

  bool queryrect(Rect& rect);

  void update();

  void toggle_autoupdate()
  {
    autoupdate = !autoupdate;
  };

  void set_zoom(float z)
  {
    zoom = MIN(5, MAX(0.5f, z));
  };

  float get_zoom()
  {
    return zoom;
  }

  int pause();

  void move(int x, int y);

  string get_name()
  {
    return title;
  }

  virtual void push(int times = 1);

  void set_output(string file, int slowdown=1);

  bool is_saving()
  {
    return !output.empty();
  };

  static void grid(int x, int y, int cellwidth, int cellheight);

protected:

  ImageWindow(string t, cv::Size size, bool aup = true);

private:

  static vector<Ptr<ImageWindow> > windows;

  string title;

  bool autoupdate;

  int output_slowdown;

  string output_file;

  Ptr<Canvas> output;

};

}

}

#endif