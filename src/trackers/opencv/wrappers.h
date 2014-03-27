

#ifndef __LEGIT_OPENCV_WRAPPERS
#define __LEGIT_OPENCV_WRAPPERS

#include "tracker.h"
#include "object_tracker.h"

using namespace cv;

namespace legit
{

namespace tracker
{

class OpenCVTracker : public Tracker
{

public:

  OpenCVTracker(Config& config, string id);

  ~OpenCVTracker();

  virtual void initialize(Image& image, cv::Rect region);

  virtual void update(Image& image);

  virtual cv::Rect region();

  virtual Point2f position();

  virtual void visualize(Canvas& canvas);

  virtual bool is_tracking();

  virtual void add_observer(Ptr<Observer> observer) {};

  virtual void remove_observer(Ptr<Observer> observer) {};

  virtual string get_name() = 0;

protected:

  Ptr<ObjectTracker> tracker;

  bool active;

  cv::Rect rectangle;

};

class OnlineBoostingTracker : public OpenCVTracker
{

public:

  OnlineBoostingTracker(Config& config, string id);

  ~OnlineBoostingTracker() {};

  virtual string get_name();

};

class MILTracker : public OpenCVTracker
{

public:

  MILTracker(Config& config, string id);

  ~MILTracker() {};

  virtual string get_name();

};

}

}

#ifdef REGISTER_TRACKER
REGISTER_TRACKER(OnlineBoostingTracker, "boosting");
REGISTER_TRACKER(MILTracker, "mil");
#endif

#endif
