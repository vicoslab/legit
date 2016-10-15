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

#ifndef GUI_H
#define GUI_H

#include "common/export.h"

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <exception>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "common/canvas.h"
#include "common/math/geometry.h"
#include "common/utils/utils.h"
#include "common/utils/config.h"
#include "common/utils/defs.h"

using namespace std;
using namespace cv;

#define VISUALDEBUG_TYPE_NULL 0
#define VISUALDEBUG_TYPE_WINDOW 1
#define VISUALDEBUG_TYPE_IMAGES 2
#define VISUALDEBUG_TYPE_VIDEO 3

namespace legit
{

namespace common
{

typedef struct
{
  int type;
  string name;
  string id;
  cv::Size size;
  float zoom;
} CanvasDescription;

__LEGIT_EXPORT void initialize_canvases();

__LEGIT_EXPORT void initialize_canvases(Config& config);

__LEGIT_EXPORT void initialize_canvases(vector<CanvasDescription> channels);

__LEGIT_EXPORT Canvas* get_canvas(const string id);

__LEGIT_EXPORT void enable_gui();

__LEGIT_EXPORT bool is_gui_enabled();

__LEGIT_EXPORT void showimage(Mat& image, string title = "Image", bool halt = true); //TODO: move to gui.h

__LEGIT_EXPORT bool queryrect(string name, cv::Rect& rect); //TODO: move to gui.h

class Widget
{
public:

  virtual void draw(Ptr<Canvas> window) = 0;

  virtual bool handle_key(int key) = 0;

};


class Menu : public Widget
{

public:

  Menu(vector<string> choices, void (*cb)(int));
  ~Menu();

  virtual void draw(Ptr<Canvas> window);

  virtual bool handle_key(int key);

private:

  unsigned int selected;

  vector<string> choices;

  void (*callback)(int);

  cv::Size menuSize;

  cv::Size buttonSize;

  Scalar foreground;

  Scalar background;

};


class ProgressBar : public Widget
{

public:

  ProgressBar(int min, int max, void (*cb)(int));
  ~ProgressBar();

  virtual void draw(Ptr<Canvas> window);

  virtual bool handle_key(int key);

private:

  int min;

  int max;

  vector<string> choices;

  void (*callback)(int);

  cv::Size menuSize;

  cv::Size buttonSize;

  Scalar foreground;

  Scalar background;

};

}

}

#endif