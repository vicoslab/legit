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

#include "common/gui/window.h"
#include "common/utils/debug.h"
#include "common/utils/utils.h"
#include "common/utils/defs.h"

namespace legit
{

namespace common
{

typedef struct
{
  Rect result;
  ImageWindow* window;
} MouseCallbackState;

void _imagewindow_mousecallback(int event, int x, int y, int flags, void *param)
{

  Rect *r = &(((MouseCallbackState*) param)->result);
  ImageWindow* w = ((MouseCallbackState*) param)->window;

  if (event != EVENT_LBUTTONDOWN) return;

  if (r->x == -1)
    {
      r->x = x;
      r->y = y;
    }
  else if (r->width == -1)
    {
      int x1 = MIN(x, r->x);
      int y1 = MIN(y, r->y);
      int x2 = MAX(x, r->x);
      int y2 = MAX(y, r->y);
      r->x = x1;
      r->y = y1;
      r->width = x2 - x1;
      r->height = y2 - y1;
    }

}

vector<Ptr<ImageWindow> > ImageWindow::windows;

ImageWindow* ImageWindow::create(string t, Size size, bool aup)
{

  ImageWindow* window = new ImageWindow(t, size, aup);

  ImageWindow::windows.push_back(Ptr<ImageWindow>(window));

  return window;

}

ImageWindow::ImageWindow(string t, Size size, bool aup) : ImageCanvas(size), title(t), autoupdate(aup), output(NULL)
{

  namedWindow(title, WINDOW_AUTOSIZE);

  if (autoupdate)
    imshow(title, image);

}

ImageWindow::~ImageWindow()
{

}



bool ImageWindow::queryrect(Rect& rect)
{

  MouseCallbackState state;

  state.result.x = -1;
  state.result.width = -1;
  state.window = this;

  setMouseCallback(title.c_str(), _imagewindow_mousecallback, (void*) &state);

  while(true)
    {
      char key = waitKey(30);
      if(key >= 0 || state.result.width > -1) break;
    }

  setMouseCallback(title.c_str(), NULL, NULL);

  if (state.result.width > -1)
    {
      rect = state.result;
      return true;
    }

  return false;
}


void ImageWindow::resize(Size size)
{
  ImageCanvas::resize(size);
  if (autoupdate)
    update();
}


void ImageWindow::grid(int rows, int columns, int width, int height)
{
  int c = 0, r = 0;

  for (int i = 0; i < windows.size(); i++)
    {

      moveWindow(windows[i]->title.c_str(), c * width, r * height);

      c++;
      if (c > columns)
        {
          c = 0;
          r++;
        }
    }


}

void ImageWindow::clear()
{
  ImageCanvas::clear();
  if (autoupdate)
    update();
}

void ImageWindow::draw(Mat& src, Point offset, int flags)
{

  ImageCanvas::draw(src, offset, flags);

  if (autoupdate)
    update();
}

void ImageWindow::rectangle(Rect r, Scalar color, int width)
{

  ImageCanvas::rectangle(r, color, width);

  if (autoupdate)
    update();
}

void ImageWindow::rectangle(Point tl, Point br, Scalar color, int width)
{

  ImageCanvas::rectangle(tl, br, color, width);

  if (autoupdate)
    update();
}

void ImageWindow::rectangle(RotatedRect r, Scalar color, int width)
{

  ImageCanvas::rectangle(r, color, width);

  if (autoupdate)
    update();

}

void ImageWindow::line(Point p1, Point p2, Scalar color, int width)
{

  ImageCanvas::line(p1, p2, color, width);

  if (autoupdate)
    update();
}

void ImageWindow::cross(Point p, Scalar color, int size)
{
  drawcross(image, p * zoom, color, size);

  if (autoupdate)
    update();
}

void ImageWindow::ellipse(Point center, Matrix2f covariance, Scalar color, int width)
{

  ImageCanvas::ellipse(center, covariance, color, width);

  if (autoupdate)
    update();
}

void ImageWindow::text(Point p, string str, Scalar color)
{

  ImageCanvas::text(p, str, color);

  if (autoupdate)
    update();
}

void ImageWindow::update()
{
  imshow(title, image);
}

int ImageWindow::pause()
{

  while(true)
    {
      char key = waitKey(30);
      if(key >= 0)
        {
          DEBUGMSG("Key pressed: %d \n", key);
          return key;
        }
    }

  return -1;
}

void ImageWindow::move(int x, int y)
{
  moveWindow(title.c_str(), x, y);
}

void ImageWindow::push(int times)
{

  update();

  if (output_file.size() == 0)
    return;

  if (output.empty())
    {
      output = new VideoCanvas(output_file, 10, cv::Size(width(), height()), true);
    }

  output->draw(image);

  output->push(times * output_slowdown);

}

void ImageWindow::set_output(string file, int slowdown)
{

  output_slowdown = MAX(1, slowdown);

  output_file = file;

  if (!output.empty())
    output.release();

}

}

}
