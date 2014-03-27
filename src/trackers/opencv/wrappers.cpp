
#include "wrappers.h"

namespace legit
{

namespace tracker
{

OpenCVTracker::OpenCVTracker(Config& config, string id)
{

  active = false;
  rectangle = Rect(0, 0, 0, 0);

}

OpenCVTracker::~OpenCVTracker()
{

}

void OpenCVTracker::initialize(Image& image, cv::Rect region)
{

  const cv::Mat gray = image.get_gray();

  DEBUGMSG("Initializing tracker \n");

  active = tracker->initialize(gray, region);

  if (!active)
    DEBUGMSG("Initialization failed \n");

  rectangle = region;

}

void OpenCVTracker::update(Image& image)
{
  if (!is_tracking())
    return;

  const cv::Mat gray = image.get_gray();

  DEBUGMSG("Updating tracker \n");

  active = tracker->update(gray, rectangle);

  if (!active)
    DEBUGMSG("Update failed \n");

}

cv::Rect OpenCVTracker::region()
{
  return rectangle;
}

Point2f OpenCVTracker::position()
{

  Point center(rectangle.x + rectangle.width / 2, rectangle.y + rectangle.height / 2);

  return center;
}

void OpenCVTracker::visualize(Canvas& canvas)
{

  canvas.rectangle(rectangle, COLOR_RED, 3);

}

bool OpenCVTracker::is_tracking()
{
  return active;
}

OnlineBoostingTracker::OnlineBoostingTracker(Config& config, string id) : OpenCVTracker(config, id)
{


  ObjectTrackerParams params;

  params.algorithm_ = ObjectTrackerParams::CV_ONLINEBOOSTING;
  params.num_classifiers_ = config.read<int>("num_classifiers", 100);
  params.overlap_ = config.read<float>("overlap", 0.99f);
  params.search_factor_ = config.read<float>("search_factor", 2.0f);

  tracker = new ObjectTracker(params);

}

string OnlineBoostingTracker::get_name()
{

  return string("Online Boosting tracker");

}

MILTracker::MILTracker(Config& config, string id) : OpenCVTracker(config, id)
{

  ObjectTrackerParams params;

  params.algorithm_ = ObjectTrackerParams::CV_ONLINEMIL;
  params.num_classifiers_ = config.read<int>("num_classifiers", 100);
  params.overlap_ = config.read<float>("overlap", 0.99f);
  params.search_factor_ = config.read<float>("search_factor", 2.0f);

  params.pos_radius_train_ = config.read<float>("pos_raidus_train", 4.0f);
  params.neg_num_train_ = config.read<int>("neg_num_train", 65);
  params.num_features_ = config.read<int>("num_features", 250);

  tracker = new ObjectTracker(params);

}

string MILTracker::get_name()
{

  return string("MIL tracker");

}

}

}
