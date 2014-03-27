/********************************************************************
* LGT tracker - The official C++ implementation of the LGT tracker
* Copyright (C) 2013  Luka Cehovin
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
********************************************************************/
/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef __LEGIT_LGT_TRACKER
#define __LEGIT_LGT_TRACKER

#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "common/utils/config.h"
#include "common/utils/utils.h"
#include "common/utils/debug.h"
#include "common/utils/defs.h"
#include "common/math/geometry.h"
#include "common/math/statistics.h"
#include "tracker.h"
#include "observers.h"
#include "patches/patchset.h"
#include "modalities/modalities.h"
#include "optimization/optimization.h"
#include "optimization/crossentropy.h"

using namespace cv;
using namespace std;
using namespace legit::common;
using namespace legit::tracker;

namespace legit
{

namespace tracker
{


class LGTTracker : public Tracker
{

public:

  LGTTracker(Config& config, string instance = "default");
  ~LGTTracker();

  virtual void initialize(Image& image, cv::Rect region);

  virtual void initialize(Image& image, Mat positions);

  virtual void update(Image& image);

  virtual cv::Rect region();

  virtual Point2f position();

  virtual bool is_tracking();

  virtual void visualize(Canvas& canvas);

  virtual void add_observer(Ptr<Observer> observer);

  virtual void remove_observer(Ptr<Observer> observer);

  virtual vector<TimeStage> get_stages();

  virtual string get_name();

  vector<cv::Point> get_patch_positions();

  virtual void track(Image& image, bool announce, bool push, DebugOutput* debug = NULL);

protected:

  virtual void stage_optimization(Image& image, bool announce, bool push, DebugOutput* debug);

  virtual void stage_update_weights(Image& image, bool announce, bool push, DebugOutput* debug);

  virtual void stage_update_modalities(Image& image, bool announce, bool push, DebugOutput* debug);

  virtual void stage_add_patches(Image& image, bool announce, bool push, DebugOutput* debug);

  void notify_observers(int channel, void* data, int flags = 0);

  void notify_stage(int stage);

  int verbosity;

  Config configuration;

  string instance;

  SizeConstraints size_constraints;

  float median_size_min, median_size_max;

  float patch_scale;

  int patches_max, patches_min;

  double patches_persistence, patches_capacity;

  double median_persistence, median_threshold;

  int probability_size;

  float merge_distance;

  float reweight_similarity;

  float reweight_distance;

  float reweight_persistence;

  float weight_remove_threshold;

  float lambda_geometry, lambda_visual;

  CrossEntropyParameters global_optimization;

  CrossEntropyParameters local_optimization;

  float optimization_global_M;

  float optimization_global_R;

  float optimization_global_S;

  float optimization_local_M;

  float sampling_threshold;

  float addition_distance;

  KalmanFilter motion;

  Patches patches;

  Modalities modalities;

  vector<Ptr<Observer> > observers;

  PatchType patch_type;

  Canvas* motionCanvas;

  Canvas* weightsCanvas;

};

void supress_noise(Mat& mat, float threshold, int window, float percent, IntegralImage* integral = NULL);

}

}

#ifdef REGISTER_TRACKER
REGISTER_TRACKER(LGTTracker, "lgt");
#endif

#endif
