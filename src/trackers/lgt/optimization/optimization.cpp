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

#include "common/utils/debug.h"
#include "common/gui/gui.h"

#include "optimization.h"
#include "../external/delaunay.h"

namespace legit
{

namespace tracker
{

OptimizationStatus::OptimizationStatus(PatchSet& patches)
{

  statuses = new PatchStatus[patches.size()];
  dimension = patches.size();

  for (int i = 0; i < dimension; i++)
    {
      statuses[i].id = patches.get_id(i);
      statuses[i].position = patches.get_position(i);
    }

  reset();
}

OptimizationStatus::~OptimizationStatus()
{

  delete [] statuses;

}

void OptimizationStatus::reset()
{

  for (int i = 0; i < dimension; i++)
    {
      statuses[i].value = 0;
      statuses[i].iterations = 0;
      statuses[i].flags = 0;
    }

}

void OptimizationStatus::toggle_flags(int i, int mask)
{

  statuses[i].flags ^= mask;

}

void OptimizationStatus::set_flags(int i, int mask)
{

  statuses[i].flags |= mask;

}

void OptimizationStatus::unset_flags(int i, int mask)
{

  statuses[i].flags &= !mask;

}

void OptimizationStatus::set(int i, Point2f position, float value, int iterations, int flags)
{

  statuses[i].position = position;

  if (value >= 0)
    statuses[i].value = value;

  if (iterations > -1)
    statuses[i].iterations = iterations;

  if (iterations > -1)
    statuses[i].flags = flags;

}

void OptimizationStatus::converged(int i, int iterations)
{

  statuses[i].iterations = iterations;
  statuses[i].flags |= OPTIMIZATION_CONVERGED;

}

void OptimizationStatus::value(int i, float val)
{

  statuses[i].value = val;

}

PatchStatus OptimizationStatus::get(int i)
{

  return statuses[i];

}

Point2f OptimizationStatus::get_position(int i)
{

  return statuses[i].position;

}


int OptimizationStatus::size()
{

  return dimension;

}

void OptimizationStatus::print()
{


  for (int i = 0; i < dimension; i++)
    {
      printf("%d (%.1f, %.1f) iterations: %d, value: %.3f [", i, statuses[i].position.x, statuses[i].position.y, statuses[i].iterations, statuses[i].value);
      if (statuses[i].flags & OPTIMIZATION_CONVERGED) printf("CONVERGED ");
      if (statuses[i].flags & OPTIMIZATION_FIXED) printf("FIXED ");
      if (statuses[i].flags & OPTIMIZATION_USED) printf("USED ");
      printf("]\n");
    }


}

DelaunayConstraints::DelaunayConstraints(PatchSet& patches)
{

  Point2f *positions = new Point2f[patches.size()];
  vector<vector<int> > neighborhoods;

  C = Mat_<double>(patches.size(),patches.size());
  // C = Mat_<float>(patches.size(),patches.size());

  for (int i = 0; i < patches.size(); i++)
    {
      positions[i] = patches.get_position(i);
      neighborhoods.push_back(vector<int>());
    }

  // determine neighborhoods using Delaunay triangulation
  delaunay_neihgbours(positions, patches.size(), neighborhoods);

  float* D ;
  D = new float[patches.size()];
  // add closest non-DT node to the neighborhood for nodes with only two neighbors
  for (int i = 0; i < patches.size(); i++)   // KAKO DELUJE TA KODA?
    {
      if (neighborhoods[i].size() > 2) continue; // tovrimo trikotnik na silo, èe sosed nima dveh sosedov
      // kako prepreèimo kolinearnost izbranih treh toèk?
      distances(positions[i], positions, patches.size(), D);

      int minNode = -1;
      float minDistance = FLT_MAX;

      for (int j = 0; j < patches.size(); j++)
        {
          if (D[j] > 0 && j != neighborhoods[i][0] && j != neighborhoods[i][1] && D[j] < minDistance)
            {
              minNode = j;
              minDistance = D[j];
            }
        }
      if (minNode >= 0) neighborhoods[i].push_back(minNode);
    }

  delete [] D;

  C.setTo(0.0);
  for (int i = 0; i < neighborhoods.size(); i++)
    {
      for (int j = 0; j < neighborhoods[i].size(); j++)
        {
          C.at<double>(i, neighborhoods[i][j]) = 1.0;
        }
    }

  delete [] positions;

}

DelaunayConstraints::~DelaunayConstraints()
{
}

float DelaunayConstraints::constraint(int i, int j)
{

  return (float)C.at<double>(i, j) ; //C.at<float>(i, j);

}

ProximityConstraints::ProximityConstraints(PatchSet& patches, float f, float t) : factor(f), threshold(t)
{

  Point2f *positions = new Point2f[patches.size()];

  for (int i = 0; i < patches.size(); i++)
    {
      positions[i] = patches.get_position(i);
    }

  D = distances(positions, patches.size());

  delete [] positions;

}

float ProximityConstraints::constraint(int i, int j)
{

  float d = D.at<float>(i, j);

  if (d > threshold) return 0;

  return exp(-d * factor);

}

}

}
