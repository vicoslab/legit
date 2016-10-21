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

#ifndef LEGIT_OPTIMIZATION
#define LEGIT_OPTIMIZATION

#include <string.h>
#include <functional>
#include <opencv2/core/core.hpp>
#include <vector>
#include "../patches/patchset.h"
#include "common/math/geometry.h"
#include "common/math/statistics.h"
#include "common/utils/utils.h"

#define OPTIMIZATION_FIXED 1
#define OPTIMIZATION_CONVERGED 2
#define OPTIMIZATION_USED 4

#define OPTIMIZATION_VISUAL_DEBUG 10

namespace legit
{

namespace tracker
{

typedef struct
{
  cv::Size min_size;
  cv::Size max_size;
} SizeConstraints;

typedef struct
{
  int id;
  Point2f position;
  int iterations;
  float value;
  int flags;
} PatchStatus;

class OptimizationStatus
{
public:
  OptimizationStatus(PatchSet& patches);
  ~OptimizationStatus();

  void reset();
  void set(int i, Point2f position, float value = -1, int iterations = -1, int flags = -1);
  void converged(int i, int iterations);
  void value(int i, float val);

  PatchStatus get(int i);
  Point2f get_position(int i);

  void toggle_flags(int i, int mask);
  void set_flags(int i, int mask);
  void unset_flags(int i, int mask);

  int size();

  void print();

private:

  int dimension;
  PatchStatus* statuses;

};

#define OPTIMIZATION_DEBUG_CANDIDATE 0
#define OPTIMIZATION_DEBUG_CONSENSUS 1
#define OPTIMIZATION_DEBUG_CONVERGED 2
/*
class OptimizationDebug {
public:
    OptimizationDebug();
    ~OptimizationDebug();

    void add_position(int i, int type, Point2f p) = 0;

    void add_covariance(int i, Matrix2f cov) = 0;

    void new_iteration() = 0;
}
*/
class ResponseFunction
{
public:

  virtual float response(int i, Point2f position) = 0;

  virtual float normalization() = 0;

};

class Constraints
{
public:

  Constraints() {};

  virtual float constraint(int i, int j) = 0;

};

class FlatConstraints : public Constraints
{
public:

  FlatConstraints() {};

  virtual float constraint(int i, int j)
  {
    return i != j ? 1 : 0;
  };

};

class DelaunayConstraints : public Constraints
{
public:

  DelaunayConstraints(PatchSet& patches);
  virtual ~DelaunayConstraints();

  virtual float constraint(int i, int j);

private:

  Matrix C;

};

class ProximityConstraints : public Constraints
{
public:

  ProximityConstraints(PatchSet& patches, float factor, float threshold);

  virtual float constraint(int i, int j);

private:

  Matrix D;

  float factor;

  float threshold;

};

}

}

#endif