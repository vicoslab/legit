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

#include "focus.h"

namespace legit
{

namespace tracker
{

FocusWrapper::FocusWrapper(Config& config, string id)
{

  if (!config.keyExists("focus.tracker"))
    throw LegitException("Unknown parent tracker type");

  tracker = create_tracker(config.read<string>("focus.tracker"), config, id);

  int fwidth = MAX(10, config.read<int>("focus.width", 150));
  int fheight = MAX(10, config.read<int>("focus.height", 150));

  DEBUGMSG("Focused wrapper enabled (%d x %d)\n", fwidth, fheight);

  window = Size(fwidth, fheight);

}

FocusWrapper::~FocusWrapper()
{

}

void FocusWrapper::initialize(Image& image, cv::Rect region)
{

  Point center(region.x + region.width / 2, region.y + region.height / 2);

  update_offset(center, image.width(), image.height());

  cropped.copy_region(image, Rect(offset, window));

  tracker->initialize(cropped, region - offset);
}
/*
void FocusWrapper::initialize(Image& image, Mat positions) {

    Point center(0, 0);

    for (int i = 0; i < positions.rows; i++) {
        center += Point2f(positions.at<float>(i, 0), positions.at<float>(i, 1));
    }

    center /= positions.rows;

    Mat rpositions;
    positions.copyTo(rpositions);

    update_offset(center, image.width(), image.height());


    for (int i = 0; i < positions.rows; i++) {
        rpositions.at<float>(i, 0) -= offset.x;
        rpositions.at<float>(i, 1) -= offset.y;
    }

    cropped.copy_region(image, Rect(offset, window));

    tracker->initialize(cropped, rpositions);
}
*/
void FocusWrapper::update(Image& image)
{

  Rect region = tracker->region() + offset;

  Point center(region.x + region.width / 2, region.y + region.height / 2);

  update_offset(center, image.width(), image.height());

  cropped.copy_region(image, Rect(offset, window));

  tracker->update(cropped);

}

cv::Rect FocusWrapper::region()
{
  return tracker->region() + offset;
}

Point2f FocusWrapper::position()
{
  return tracker->position() + offset;
}

void FocusWrapper::visualize(Canvas& canvas)
{

  ProxyCanvas pcanvas(&canvas, offset);

  tracker->visualize(pcanvas);

  Rect r(offset, window);

  canvas.rectangle(r, COLOR_RED, 3);

}

void FocusWrapper::update_offset(Point center, int img_width, int img_height)
{

  Point move(center.x - window.width / 2, center.y - window.height / 2);

  offset.x = MAX(0, MIN(img_width - window.width, move.x));
  offset.y = MAX(0, MIN(img_height - window.height, move.y));

}

}

}
