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

#include "common/utils/string.h"
#include "common/gui/gui.h"
#include "common/gui/window.h"

namespace legit
{

namespace common
{

typedef struct
{
  string id;
  Canvas* canvas;
} NamedCanvas;

Canvas dummy_canvas;

vector<NamedCanvas> canvases;

bool gui_enabled = false;

void enable_gui()
{
  gui_enabled = true;
}

bool is_gui_enabled()
{
  return gui_enabled;
}

void initialize_canvases()
{

}

void initialize_canvases(Config& config)
{

  vector<CanvasDescription> descriptions;

//  char tmpconfig[256];

  vector<string> config_keys = config.keys();

  for( std::vector<string>::iterator key = config_keys.begin(); key != config_keys.end(); ++key)
    {

      if (!matches_prefix((*key).c_str(), "canvas."))
        continue;

      if ((*key).size() < 8 || ((*key).rfind('.') != 6))
        continue;

      CanvasDescription cd;

      string canvas_key = *key;

      cd.id = canvas_key.substr(7, string::npos);

      string type = config.read<string>(canvas_key, "none");

      if (type == "window" && gui_enabled)
        {
          cd.type = VISUALDEBUG_TYPE_WINDOW;
        }
      else if (type == "video")
        {
          cd.type = VISUALDEBUG_TYPE_VIDEO;
        }
      else if (type == "images")
        {
          cd.type = VISUALDEBUG_TYPE_IMAGES;
        }
      else cd.type = VISUALDEBUG_TYPE_NULL;

      DEBUGMSG("Registered canvas '%s' of type %s \n", cd.id.c_str(), type.c_str());

      std::stringstream sstm;
      sstm << "Canvas '" << cd.id << "'";

//        sprintf(tmpconfig, "window.channel%d.name", channel);
      cd.name = config.read<string>(canvas_key + ".name", sstm.str());

      //sprintf(tmpconfig, "window.channel%d.zoom", channel);
      cd.zoom = MIN(5, MAX(0.5f, config.read<float>(canvas_key + ".zoom", 1.0)));

      //sprintf(tmpconfig, "window.channel%d.width", channel);
      cd.size.width = config.read<int>(canvas_key + ".width", 320) * cd.zoom;
      cd.size.width = MAX(1, cd.size.width);

      //sprintf(tmpconfig, "window.channel%d.height", channel);
      cd.size.height = config.read<int>(canvas_key + ".height", 240) * cd.zoom;
      cd.size.height = MAX(1, cd.size.height);

      descriptions.push_back(cd);

    }

  /*

      while (true) {

          channel++;

          sprintf(tmpconfig, "window.channel%d", channel);

          VisualDebugChannelDescription cd;

          if (!config.keyExists(tmpconfig)) {
              if (!(channel < 32)) // search at least 32 channels
                  break;
          }

          string type = config.read<string>(tmpconfig, "none");

          if (type == "window" && gui_enabled) {
              cd.type = VISUALDEBUG_TYPE_WINDOW;
          } else if (type == "video") {
              cd.type = VISUALDEBUG_TYPE_VIDEO;
          } else if (type == "images") {
              cd.type = VISUALDEBUG_TYPE_IMAGES;
          } else cd.type = VISUALDEBUG_TYPE_NULL;

          DEBUGMSG("Registered debug channel %d of type %s \n", channel, type.c_str());

          std::stringstream sstm;
          sstm << "Debug channel " << channel;

          sprintf(tmpconfig, "window.channel%d.name", channel);
          cd.name = config.read<string>(tmpconfig, sstm.str());

          sprintf(tmpconfig, "window.channel%d.zoom", channel);
          cd.zoom = MIN(5, MAX(0.5f, config.read<float>(tmpconfig, 1.0)));

          sprintf(tmpconfig, "window.channel%d.width", channel);
          cd.size.width = config.read<int>(tmpconfig, 320) * cd.zoom;
          cd.size.width = MAX(1, cd.size.width);

          sprintf(tmpconfig, "window.channel%d.height", channel);
          cd.size.height = config.read<int>(tmpconfig, 240) * cd.zoom;
          cd.size.height = MAX(1, cd.size.height);

          channels.push_back(cd);
      DEBUGMSG("VD: %s\n", cd.name.c_str());
      }*/

  initialize_canvases(descriptions);

}

void initialize_canvases(vector<CanvasDescription> descriptions)
{

  for (int i = 0; i < descriptions.size(); i++)
    {
      Canvas* canvas = NULL;
      switch(descriptions[i].type)
        {
        case VISUALDEBUG_TYPE_WINDOW:
          canvas = ImageWindow::create(descriptions[i].name, descriptions[i].size, false);
          break;
        case VISUALDEBUG_TYPE_VIDEO:
          canvas = new VideoCanvas(descriptions[i].name, 10, descriptions[i].size, true); // TODO: this is most likely not released !!!! memory leak !!!!
          break;
          /*        case VISUALDEBUG_TYPE_IMAGES:
                      debug_channels.push_back(ImageWindow::create(channels[i].name, channels[i].size, false));
                      break;*/
        default:
          canvas = &dummy_canvas;
          break;
        }

      if (canvas)
        {
          NamedCanvas named_canvas;
          named_canvas.id = descriptions[i].id;
          canvas->set_zoom(descriptions[i].zoom);
          named_canvas.canvas = canvas;
          canvases.push_back(named_canvas);
        }
    }

}

void showimage(Mat& image, string title, bool halt)
{

  namedWindow(title,WINDOW_AUTOSIZE);
  imshow(title, image);

  if (halt)
    {
      while(true)
        {
          char key = waitKey(30);
          if(key >= 0) break;
        }
    }
}


void _mousecallback(int event, int x, int y, int flags, void *param)
{

  if (event != EVENT_LBUTTONDOWN) return;

  Rect *r = (Rect*) param;

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

bool queryrect(string name, Rect& rect)
{

  Rect r;

  r.x = -1;
  r.width = -1;

  setMouseCallback(name.c_str(), _mousecallback, (void*) &r);

  while(true)
    {
      char key = waitKey(30);
      if(key >= 0 || r.width > -1) break;
    }

  setMouseCallback(name.c_str(), NULL, NULL);

  if (r.width > -1)
    {
      rect = r;
      return true;
    }

  return false;
}

Canvas* get_canvas(const string id)
{

  if (id.size() == 0)
    return &dummy_canvas;

  for( std::vector<NamedCanvas>::iterator c = canvases.begin(); c != canvases.end(); ++c)
    {

      if ((*c).id.compare(id) == 0)
        return (*c).canvas;

    }

  return &dummy_canvas;
}

#define __PANELMARGIN 10
#define __BUTTONMARGIN 5

Menu::Menu(vector<string> ch, void (*cb)(int)) : selected(0), menuSize(0, 0), buttonSize(0, 0)
{

  foreground = Scalar(255,255,255);
  background = Scalar(0,0,0);

  for (int i = 0; i < ch.size(); i++)
    {
      int baseline = 0;
      Size sz = getTextSize(ch[i].c_str(), FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseline);

      buttonSize.width = MAX(buttonSize.width, sz.width);
      buttonSize.height = MAX(buttonSize.height, sz.height);
      choices.push_back(choices[i]);
    }

  menuSize.width = buttonSize.width + __PANELMARGIN * 2;
  menuSize.height = buttonSize.height * choices.size() + (__BUTTONMARGIN * (choices.size() - 1)) + __PANELMARGIN * 2;

}

Menu::~Menu()
{


}

void Menu::draw(Ptr<Canvas> window)
{

  Point offset;
  offset.x = (window->width() - menuSize.width) / 2;
  offset.y = (window->height() - menuSize.height) / 2;

  window->rectangle(offset, offset + menuSize, background, CV_FILLED);
  window->rectangle(offset, offset + menuSize, foreground, 1);

  Point buttonOffset = offset + __PANELMARGIN;

  for (int i = 0; i < choices.size(); i++)
    {
//      int baseline = 0;
//      Size sz = getTextSize(choices[i].c_str(), FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseline);

      Point textOffset = buttonOffset;
//        textOffset.x += sz

      window->rectangle(buttonOffset, buttonOffset + buttonSize, i == selected ? foreground : background, CV_FILLED);
      window->rectangle(buttonOffset, buttonOffset + buttonSize, i == selected ? background : foreground, 1);

      window->text(textOffset, choices[i].c_str(), i == selected ? background : foreground);

      buttonOffset.y += buttonSize.height + __BUTTONMARGIN;

    }

}

bool Menu::handle_key(int key)
{

  if (key < 0 ) return false;

  switch (key)
    {
    case 81:
      break;
    case 83:
      break;
    case 82:
      //move(current_frame + 10);
      break;
    case 84:
      //move(current_frame - 10);
      break;
    default:
      return false;
    }

  return true;

}

}

}