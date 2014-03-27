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

#ifndef LEGIT_PATHCES
#define LEGIT_PATHCES

#include <string.h>
#include <functional>
#include <opencv2/core/core.hpp>
#include "common/utils/buffer.h"
#include "common/math/geometry.h"
#include "common/image/image.h"
#include "common/image/histogram.h"

using namespace cv;
using namespace std;
using namespace legit::common;

namespace legit
{

namespace tracker
{

enum PatchType {HISTOGRAM, SSD, RGBPIXEL, HSPIXEL, PATCH_TYPE_COUNT, PATCH_TYPE_ANY};

typedef struct _State
{
  Point2f position;
  float weight;
  bool active;
} State;

class Patch
{
public:
  Patch(int id, int capacity, int limit, int width, int height) : states(capacity, limit), age(0), id(id), width(width), height(height), active(true) {}
  ~Patch() {}

  inline Point2f get_position(int offset)
  {
    return states.get(offset).position;
  }

  inline float get_weight(int offset)
  {
    return states.get(offset).weight;
  }

  inline Point2f get_position()
  {
    return states.get().position;
  }

  inline float get_weight()
  {
    return states.get().weight;
  }

  void set_position(Point2f p);
  void set_weight(float weight);
  void move_position(Point2f p);
  inline int get_age()
  {
    return age;
  };
  inline int get_id()
  {
    return id;
  };
  inline int get_width()
  {
    return width;
  };
  inline int get_height()
  {
    return height;
  };
  inline bool is_active()
  {
    return active;
  };
  inline void set_active(bool a)
  {
    active = a;
  };
  inline bool toggle_active()
  {
    active = !active;
    return active;
  };

  void push();
  int history_size()
  {
    return states.size();
  };


  virtual void initialize(Image& image, cv::Point position) = 0;
  virtual float response(Image& image, cv::Point position) = 0;
  virtual void responses(Image& image, cv::Point2f* positions, int pcount, float* responses) = 0;
  virtual PatchType get_type() = 0;

protected:

  Buffer<State> states;
  int age;
  int id;
  int width;
  int height;
  bool active;

};

struct Filter : public unary_function<Ptr<Patch>&,bool>
{
  virtual bool operator() (Ptr<Patch>& patch)
  {
    return true;
  };
};

class WeightGreaterFilter : public Filter
{
public:
  WeightGreaterFilter(float threshold) : threshold(threshold) {}
  ~WeightGreaterFilter() {}
  virtual bool operator() (Ptr<Patch>& patch)
  {
    return (patch->get_weight(0) > threshold);
  }
private:
  float threshold;
};

class WeightLowerFilter : public Filter
{
public:
  WeightLowerFilter(float threshold) : threshold(threshold) {}
  ~WeightLowerFilter() {}
  virtual bool operator() (Ptr<Patch>& patch)
  {
    return (patch->get_weight(0) < threshold);
  }
private:
  float threshold;
};

class ActiveFilter : public Filter
{
public:
  ActiveFilter() {}
  ~ActiveFilter() {}
  virtual bool operator() (Ptr<Patch>& patch)
  {
    return patch->is_active();
  }
};

class PatchSet
{
public:

  PatchSet(int size);

  PatchSet(PatchSet& set);

  virtual ~PatchSet();

  virtual int size();

  virtual float get_weight(int index);

  virtual Point2f get_position(int index, int offset = 0);

  virtual Point2f get_relative_position(int index, Point2f origin);

  virtual PatchType get_type(int index);

  virtual bool is_active(int index);

  virtual int get_age(int index);

  virtual int get_id(int index);

  virtual void set_weight(int index, float weight);

  virtual void set_position(int index, Point2f position);

  virtual void set_active(int index, bool active);

  virtual Point2f mean_position(bool weighted = true);

  virtual Matrix2f position_covariance(bool weighted = true);

  virtual Rect4f region();

  virtual void print(int i);

  virtual void print_all();

  float response(Image& matrix, int index, Point2f position);

  void responses(Image& matrix, int index, Point2f* positions, int pcount, float* responses);

  int get_patch_size()
  {
    return psize;
  }

  int get_radius()
  {
    return psize / 2;
  }

  PatchSet* filter(Filter& filter);

protected:

  vector<Ptr<Patch> > patches;

  int hsize;
  int psize;

};

class Patches : public PatchSet
{

public:

  Patches(int size, int bufferlimit = 5);

  virtual ~Patches();

  void push();

  void move(Point2f vector);

  int add(Image& image, PatchType type, Point2f position, float weight);

  void remove(int index);

  void remove(vector<int>& indices);

  int remove(Filter& filter);

  void flush();

  int merge(Image& image, vector<int>& indices, PatchType type = PATCH_TYPE_ANY);

  int inhibit(Image& image, vector<int>& indices);

  int get_motion_history(int index, Point2f* buffer, int maxlen);

  void normalize_weights();

  void set_patch_size(int size)
  {
    flush();
    psize = size;
  }

private:

  int count;
  int bufferlimit;

};

}

}

#endif