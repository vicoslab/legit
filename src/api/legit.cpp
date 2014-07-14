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

#include "api/legit.h"
#include "tracker.h"
#include "common/utils/debug.h"

using namespace legit::tracker;

bool __random_init = false;

class LegitTracker::Impl
{
public:
  Impl() : image(), initialized(false), tracker(NULL)
  {

    if (!__random_init)
      {
        RANDOM_SEED(time(NULL));
        __random_init = true;
      }

  }
  ~Impl() {}

  Ptr<Tracker> tracker;

  Image image;

  bool initialized;
};

LegitTracker::LegitTracker(const char*  config, const char*  id)
{

  impl = new Impl();

  Config cfg;
  istringstream configStream(config);
  configStream >> cfg;


  if (!cfg.keyExists("tracker"))
    throw LegitException("Unknown tracker type");

  impl->tracker = create_tracker(cfg.read<string>("tracker"), cfg, id);

  if (!impl->tracker)
    throw LegitException("Unable to create tracker");
}

LegitTracker::~LegitTracker()
{


}

void LegitTracker::clear_image()
{

  impl->image.reset();

}

void LegitTracker::update_image(Mat& image, int imagetype)
{

  if (imagetype > -1)
    impl->image.update(image, imagetype, false);
  else
    impl->image.update(image, false);

}

void LegitTracker::initialize(cv::Rect region)
{

  if (impl->image.empty())
    return;

  impl->tracker->initialize(impl->image, region);

  impl->initialized = true;

}

void LegitTracker::update()
{

  if (!impl->initialized)
    return;

  if (impl->image.empty())
    return;

  impl->tracker->update(impl->image);

}

void LegitTracker::initialize(Mat& image, cv::Rect region)
{

  impl->image.reset();
  update_image(image, -1);

  initialize(region);

}

void LegitTracker::update(Mat& image)
{

  impl->image.reset();
  update_image(image, -1);

  update();

}

cv::Rect LegitTracker::region()
{

  return impl->tracker->region();

}

cv::Point2f LegitTracker::position()
{

  return impl->tracker->position();

}

bool LegitTracker::is_tracking()
{

  return impl->tracker->is_tracking();

}

void LegitTracker::visualize(Mat& img)
{

  if (!impl->initialized)
    return;

  impl->image.get_rgb().copyTo(img);

  ImageCanvas canvas(img);

  impl->tracker->visualize(canvas);

}

string LegitTracker::get_name()
{

  return impl->tracker->get_name();

}


void LegitTracker::set_property(int code, float value)
{

  impl->tracker->set_property(code, value);

}

float LegitTracker::get_property(int code)
{

  return impl->tracker->get_property(code);

}

void LegitTracker::remove_property(int code)
{

  impl->tracker->remove_property(code);

}

bool LegitTracker::has_property(int code)
{

  return impl->tracker->has_property(code);

}

struct CLegitTracker : public LegitTracker {};

extern "C" {

  CLegitTracker * legit_tracker_create(const char *config)
  {
    return (CLegitTracker *)new LegitTracker(config);
  }

  void legit_tracker_destroy(CLegitTracker *t)
  {
    delete t;
  }

  void legit_tracker_initialize(CLegitTracker *t, const CvMat* image, CvRect region)
  {
    cv::Mat mat(image);
    t->initialize( mat, (cv::Rect)region);
  }

  void legit_tracker_update(CLegitTracker *t, const CvMat* image)
  {
    cv::Mat mat(image);
    t->update(mat);
  }

  CvRect legit_tracker_region(CLegitTracker *t)
  {
    return t->region();
  }

  CvPoint2D32f legit_tracker_position(CLegitTracker *t)
  {
    return (CvPoint2D32f) t->position();
  }

  int legit_tracker_is_tracking(CLegitTracker *t)
  {
    if(t->is_tracking())
      {
        return 1;
      }
    else
      {
        return 0;
      }
  }

  void legit_tacker_visualize(CLegitTracker *t, const CvMat* canvas)
  {
    cv::Mat mat(canvas);
    t->visualize(mat);
  }

  const char* legit_tracker_get_name(CLegitTracker *t)
  {
    return t->get_name().c_str();
    //return "test";
    //return reinterpret_cast<const char*>(t->get_name());
  }

  void legit_set_property(CLegitTracker *t, int code, float value)
  {

    t->set_property(code, value);

  }

  float legit_get_property(CLegitTracker *t, int code)
  {

    return t->get_property(code);

  }

  void legit_remove_property(CLegitTracker *t, int code)
  {

    t->remove_property(code);

  }

  int legit_has_property(CLegitTracker *t, int code)
  {

    return t->has_property(code) ? 1 : 0;

  }

  int legit_toggle_debugging()
  {

    if (__is_debug_enabled())
      __debug_disable();
    else
      __debug_enable();

    return __is_debug_enabled();

  }

}
