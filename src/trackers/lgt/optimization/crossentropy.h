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

#ifndef LEGIT_OPTIMIZATION_CE
#define LEGIT_OPTIMIZATION_CE

#include "optimization.h"

namespace legit
{

namespace tracker
{

class CrossEntropyParameters
{
public:
  CrossEntropyParameters(int min_s, int max_s, int add_s, int elite_s, int iter, float term) : max_samples(max_s),
    min_samples(min_s), add_samples(add_s), elite_samples(elite_s), iterations(iter), terminate(term) {};

  int max_samples;
  int min_samples;
  int add_samples;
  int elite_samples;
  int iterations;
  float terminate;
};
/*
template <class T>
class OrderedBoundedBuffer {
private:
    int last;
    int _capacity;
    int _limit;
    bool full;
    bool empty;
    T *buffer;
    float *scores;
    int *previous;

public:
    /
        capacity: current capacity
        limit:  max size
    /
    OrderedBoundedBuffer(int capacity) {
        _capacity = capacity;
        full = false;
        empty = true;
        buffer = new T[capacity];
        previous = new int[capacity];
        scores = new float[capacity];
        last = -1;
        bottom = -1;
    }

    ~OrderedBoundedBuffer() {
        delete [] buffer;
        delete [] scores;
        delete [] previous;
    }

    void push(T value, float score) {

        if (!full) {
        	int i = bottom;
        	int j = -1;

        	bottom++;
        	while (i != -1) {
        		if (scores[i] > score)
        			break;
        		j = i;
        		i = previous[i];
        	}

        	buffer[bottom] = value;
        	scores[bottom] = score;

        	if (i != -1) {
        		previous[bottom] = i;
        		if (j != -1) previous[j] = bottom;
        	} else {
        		previous[bottom] = -1;
        		if (j != -1) previous[j] = bottom;
        	}


            full = bottom == _capacity;

        } else {



        }
    }

    int size() {
        return full ? _capacity : last + 1;
    }

    bool is_full() {
        return full;
    }

    int capacity() {
        return _capacity;
    }

    void flush() {
        last = -1;
        full = false;
    }

    T* array() {

        if (size() == 0)
            return NULL;

        T* result = malloc(sizeof(T) * size());

        int pos = start;
        int i = 0;
        while (pos != end) {
            result[i] = buffer[pos];

            pos++;
            i++;

            if (pos >= _capacity) {
                pos = 0;
            }
        }

        return result;
    }

};
*/
void cross_entropy_global_move(Image& image, PatchSet& patches, Matrix& mean, Matrix& covariance, CrossEntropyParameters params, OptimizationStatus& status);

void cross_entropy_global_affine(Image& image, PatchSet& patches, Matrix& mean, Matrix& covariance, CrossEntropyParameters params, SizeConstraints size_constraints, OptimizationStatus& status);

void cross_entropy_global_affine2(OptimizationStatus& status, ResponseFunction& function, Matrix& mean, Matrix& covariance, CrossEntropyParameters params, SizeConstraints size_constraints);

//void cross_entropy_global_affine2(ResponseMaps& patches, Matrix& mean, Matrix& covariance, CrossEntropyParameters params, OptimizationStatus* status);

void cross_entropy_local_refine(Image& image, PatchSet& patches, Constraints& constraints, float covariance, float lambda_geometry, float lambda_visual, CrossEntropyParameters params, OptimizationStatus& status);

//void cross_entropy_local_refine2(Image& image, PatchSet& patches, Constraints& constraints, float covariance, float lambda_geometry, float lambda_visual, CrossEntropyParameters params, OptimizationStatus* status);

}

}

#endif