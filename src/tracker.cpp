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

#include "tracker.h"
#include "common/utils/defs.h"
#include "common/gui/gui.h"

#ifdef BUILTIN_TRACKERS
#include BUILTIN_TRACKERS
#endif

#ifdef BUILTIN_CONFIG
#include BUILTIN_CONFIG
#endif

namespace legit
{

namespace tracker
{
/*
vector<legit::tracker::TrackerFactory*> tracker_registry;

void register_tracker(TrackerFactory* factory) {

    if (factory)
        tracker_registry.push_back(factory);

}*/

Tracker* create_tracker(string type, Config& config, string id)
{

  /*if (!config.keyExists("tracker"))
      throw LegitException("Unknown tracker type");
  */
  Tracker* ptr = NULL;
  //string type = config.read<string>("tracker");

  TrackerRegistry& reg = getTrackerRegistry();
  TrackerRegistry::iterator it = reg.find(type);

  DEBUGMSG("Creating tracker %s\n", type.c_str());

  if (it != reg.end())
    {
      CreateTrackerFunc func = it->second;
      ptr = func(config, id);
    }

  if (!ptr)
    throw LegitException("Unknown tracker type");

  DEBUGMSG("Tracking algorithm: %s\n", type.c_str());

  return ptr;
  /*
      if (config.read<bool>("tracker.focus", false)) {
          int fwidth = MAX(10, config.read<int>("tracker.focus.width", 150));
          int fheight = MAX(10, config.read<int>("tracker.focus.height", 150));

          DEBUGMSG("Focused wrapper enabled (%d x %d)\n", fwidth, fheight);

          return new ProxyTracker(ptr, Size(fwidth, fheight));
      } else {
          return ptr;
      }
  */
}

vector<string> list_builtin_configs()
{

  vector<string> list;

#ifdef BUILTIN_CONFIG

  int i = 0;
  while (!_legit_builtin_config[i].empty())
    {
      // string name(_legit_defaul_config[i]);
      list.push_back(_legit_builtin_config[i]);
      i += 2;
    }

#endif

  return list;
}

vector<string> list_registered_trackers()
{

  vector<string> list;

  TrackerRegistry& reg = getTrackerRegistry();

  for (TrackerRegistry::iterator it = reg.begin(); it != reg.end(); ++it)
    list.push_back(it->first);

  return list;
}

void read_builtin_config(string name, Config& config)
{

#ifdef BUILTIN_CONFIG

  int i = 0;
  while (!_legit_builtin_config[i].empty())
    {
      if (_legit_builtin_config[i] == name)
        {
          istringstream defaultConfig(_legit_builtin_config[i+1]);
          defaultConfig >> config;
          break;
        }
      i += 2;
    }

#endif

}

void Tracker::set_property(int code, float value)
{

  properties[code] = value;

}

float Tracker::get_property(int code)
{

  map<int, float>::iterator it = properties.find(code);

  if (it == properties.end())
    throw LegitException("Property not available");

  return it->second;

}

void Tracker::remove_property(int code)
{

  properties.erase(code);

}

bool Tracker::has_property(int code)
{

  map<int, float>::iterator it = properties.find(code);

  return (it != properties.end());

}

void ProxyTracker::initialize(Image& image, cv::Rect region)
{

  tracker->initialize(image, region);
}

void ProxyTracker::update(Image& image)
{

  tracker->update(image);

}

cv::Rect ProxyTracker::region()
{
  return tracker->region();
}

Point2f ProxyTracker::position()
{
  return tracker->position();
}

bool ProxyTracker::is_tracking()
{
  return tracker->is_tracking();
}

void ProxyTracker::visualize(Canvas& canvas)
{

  tracker->visualize(canvas);
}

void ProxyTracker::add_observer(Ptr<Observer> observer)
{
  tracker->add_observer(observer);
}

void ProxyTracker::remove_observer(Ptr<Observer> observer)
{
  tracker->remove_observer(observer);
}

vector<TimeStage> ProxyTracker::get_stages()
{
  return tracker->get_stages();
}

string ProxyTracker::get_name()
{
  return tracker->get_name();
}

void ProxyTracker::set_property(int code, float value)
{

  tracker->set_property(code, value);

}

float ProxyTracker::get_property(int code)
{

  return tracker->get_property(code);

}

void ProxyTracker::remove_property(int code)
{

  tracker->remove_property(code);

}

bool ProxyTracker::has_property(int code)
{

  return tracker->has_property(code);

}

}

}