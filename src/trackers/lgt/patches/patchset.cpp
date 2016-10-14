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

#include <stdio.h>
#include "patchset.h"
#include "patch.h"

namespace legit
{

namespace tracker
{

bool _isempty (Ptr<Patch> i)
{
  return i.empty();
}

void Patch::set_position(Point2f p)
{
  State s = states.get(0);
  s.position = p;
  states.set(s);
}

void Patch::move_position(Point2f m)
{
  State s = states.get(0);
  s.position += m;
  states.set(s);
}


void Patch::set_weight(float weight)
{
  State s = states.get(0);
  s.weight = weight;
  states.set(s);
}

void Patch::push()
{

  if (states.size() == 0)
    {
      State s;
      s.position.x = 0;
      s.position.y = 0;
      s.weight = 0;
      s.active = active;
      states.push(s);
    }
  else states.push(states.get(0));
  age++;

}

PatchSet::PatchSet(int size) : patches(), psize(size)
{

}

PatchSet::PatchSet(PatchSet& set)
{

  patches.reserve(set.size());

  for (int i = 0; i < size(); i++)
    {
      patches.push_back(set.patches[i]);
    }

}

PatchSet::~PatchSet()
{

}

int PatchSet::size()
{
  return patches.size();

}

float PatchSet::get_weight(int index)
{

  return patches[index]->get_weight(0);

}


Point2f PatchSet::get_position(int index, int offset)
{

  return patches[index]->get_position(offset);

}

Point2f PatchSet::get_relative_position(int index, Point2f origin)
{

  Point2f out;
  Point2f p = patches[index]->get_position();
  out.x = p.x - origin.x;
  out.y = p.y - origin.y;

  return out;
}

bool PatchSet::is_active(int index)
{
  return patches[index]->is_active();
}

int PatchSet::get_age(int index)
{

  return patches[index]->get_age();

}

int PatchSet::get_id(int index)
{

  return patches[index]->get_id();

}

PatchType PatchSet::get_type(int index)
{

  return patches[index]->get_type();

}

void PatchSet::set_weight(int index, float weight)
{

  patches[index]->set_weight(weight);

}

void PatchSet::set_position(int index, Point2f position)
{

  patches[index]->set_position(position);

}

void PatchSet::set_active(int index, bool active)
{
  patches[index]->set_active(active);
}


Point2f PatchSet::mean_position(bool weighted)
{

  Point2f mean;
  float wsum = 0;

  for (int i = 0; i < patches.size(); i++)
    {
      float w = weighted ? patches[i]->get_weight() : 1;
      Point2f p = patches[i]->get_position();
      mean.x += p.x * w;
      mean.y += p.y * w;
      wsum += w;
    }

  mean.x /= wsum;
  mean.y /= wsum;

  return mean;
}

Matrix2f PatchSet::position_covariance(bool weighted)
{

  Matrix pt = Mat::zeros(patches.size(), 2, CV_32F);
  Matrix we = Mat::zeros(patches.size(), 1, CV_32F);

  for (int i = 0; i < patches.size(); i++)
    {
      Point2f p = patches[i]->get_position();
      pt(i, 0) = p.x;
      pt(i, 1) = p.y;
      we(i, 0) = weighted ? patches[i]->get_weight() : 1;
    }

  Matrix cov = row_weighted_covariance(pt, we);

  Matrix2f result;

  result.m00 = cov(0, 0);
  result.m10 = cov(0, 1);
  result.m01 = cov(1, 0);
  result.m11 = cov(1, 1);

  return result;
}

Rect4f PatchSet::region()
{

  if (patches.size() == 0)
    {
      return Rect4f(0, 0, 0, 0);
    }

  Rect4f r;
  r.x = INT_MAX;
  r.y = INT_MAX;
  int x2 = 0, y2 = 0;
  for (int i = 0; i < patches.size(); i++)
    {
      Point2f p = patches[i]->get_position();
      r.x = MIN(p.x, r.x);
      r.y = MIN(p.y, r.y);
      x2 = MAX(p.x, x2);
      y2 = MAX(p.y, y2);
    }

  r.width = x2 - r.x;
  r.height = y2 - r.y;

  return r;
}

void PatchSet::print(int i)
{

  //print_histogram(patches[i]->histogram);

}

void PatchSet::print_all()
{

  printf("\n*** Patches object summary: %d patches ***\n\n", (int)patches.size());

  for (int i = 0; i < patches.size(); i++)
    {
      Point2f p = patches[i]->get_position();
      printf("Position = (%.2f, %.2f), Weight = %.2f, Age = %d\n", p.x, p.y,  patches[i]->get_weight(0),  patches[i]->get_age());
    }
}

float PatchSet::response(Image& image, int index, Point2f position)
{

  return patches[index]->response(image, position);

}

void PatchSet::responses(Image& image, int index, Point2f* positions, int pcount, float* responses)
{

  patches[index]->responses(image, positions, pcount, responses);

}


Patches::Patches(int size, int limit) : PatchSet(size), count(0), bufferlimit(limit)
{
  /*
      if (t == "histogram")
          type = HISTOGRAM;
      else if (t == "ssd")
          type = SSD;
  	else if (t == "rgbpixel")
          type = RGBPIXEL;
  	else if (t == "hspixel")
          type = HSPIXEL;
      else throw LegitException("Unknown type");

  */

}

Patches::~Patches()
{

}

void Patches::push()
{

  for (int i = 0; i < patches.size(); i++)
    {
      patches[i]->push();
    }

}

void Patches::move(Point2f vector)
{

  for (int i = 0; i < patches.size(); i++)
    patches[i]->move_position(vector);

}


int Patches::add(Image& image, PatchType type, Point2f position, float weight)
{

  Ptr<Patch> pch;

  switch (type)
    {
    case HISTOGRAM:
      pch = new HistogramPatch(count++, 10, bufferlimit, psize, psize);
      break;
    case RGBPIXEL:
      pch = new RGBPatch(count++, 10, bufferlimit);
      break;
    case HSPIXEL:
      pch = new HSPatch(count++, 10, bufferlimit);
      break;
    case SSD:
      pch = new SSDPatch(count++, 10, bufferlimit, psize, psize);
      break;
    default:
      throw LegitException("Unknown type");
    }

  pch->push();
  pch->set_position(position);
  pch->set_weight(weight);

  pch->initialize(image, position);

  //calculate_histogram(image, position, psize, pch->histogram);

  patches.push_back(pch);

  return patches.size();
}

void Patches::remove(int index)
{

  patches.erase(patches.begin() + index);

}

void Patches::remove(vector<int>& indices)
{
  for (int i = 0; i < indices.size(); i++)
    {
      patches[indices[i]].release();
    }

  patches.erase(remove_if(patches.begin(), patches.end(), _isempty), patches.end());

}

int Patches::remove(Filter& filter)
{
  vector<int> selection;
  for (int i = 0; i < size(); i++)
    {
      if (filter(patches[i])) selection.push_back(i);
    }

  if (selection.size() > 0)
    {
      remove(selection);
      return selection.size();
    }
  else return 0;
}

void Patches::flush()
{
  patches.clear();
}

int Patches::merge(Image& image, vector<int>& indices, PatchType type)
{

  if (indices.size() < 2)
    return patches.size();

  Point2f p(0, 0);
  float w = 0;

  vector<PatchType> types;

  for (int i = 0; i < indices.size(); i++)
    {
      Point2f m = patches[indices[i]]->get_position();
      float mw = patches[indices[i]]->get_weight();
      p.x += m.x * mw;
      p.y += m.y * mw;
      w += mw;
      types.push_back(patches[indices[i]]->get_type());
      // a hacky way to remove patches with as little fuss as possible
      patches[indices[i]].release();
    }

  patches.erase(remove_if(patches.begin(), patches.end(), _isempty), patches.end());

  p.x /= w;
  p.y /= w;
  w /= indices.size();

  assert(type < PATCH_TYPE_COUNT);

  add(image, type, p, w);

  return patches.size();
}

int Patches::inhibit(Image& image, vector<int>& indices)
{

  if (indices.size() < 2)
    return patches.size();

  int best = -1;
  float best_weight = 0;

  for (int i = 0; i < indices.size(); i++)
    {
      if (patches[indices[i]]->get_weight() > best_weight)
        {
          best = indices[i];
          best_weight = patches[indices[i]]->get_weight();
        }
    }

  for (int i = 0; i < indices.size(); i++)
    {
      if (indices[i] == best) continue;
      // a hacky way to remove patches with as little fuss as possible
      patches[indices[i]].release();
    }

  patches.erase(remove_if(patches.begin(), patches.end(), _isempty), patches.end());

  return patches.size();
}

int Patches::get_motion_history(int index, Point2f* buffer, int maxlen)
{

  int size = MIN(patches[index]->history_size(), maxlen);

  for (int i = 0; i < size; i++)
    {
      buffer[i] = patches[index]->get_position(i);
    }

  return size;
}

void Patches::normalize_weights()
{

  float total = 0;

  for (int i = 0; i < size(); i++)
    {
      total += get_weight(i);
    }

  for (int i = 0; i < size(); i++)
    {
      set_weight(i, get_weight(i) / total);
    }

}

PatchSet* PatchSet::filter(Filter& filter)
{

  PatchSet* patchSet = new PatchSet(psize);
  patchSet->patches.reserve(size());

  for (int i = 0; i < size(); i++)
    {
      if (filter(patches[i]))
        {
          patchSet->patches.push_back(patches[i]);
        }
    }

  return patchSet;
}

}

}

