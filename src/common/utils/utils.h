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

#ifndef UTILS_H
#define UTILS_H

#include "common/export.h"

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <exception>
#include <opencv2/core/core.hpp>

#define PRINT_SIZE(s) printf("Size: %d x %d \n", s.width, s.height)

using namespace cv;
using namespace std;

#define OPERATION_SET 1
#define OPERATION_ADD 2
#define OPERATION_SUBSTRACT 3
#define OPERATION_MULTIPLY 4

#define ORIGIN_CENTER 1
#define ORIGIN_TOPLEFT 2
#define ORIGIN_BOTTOMRIGHT 2

#define PATCH_DISC 1
#define PATCH_CONE 2
#define PATCH_GAUSS 3

#define FLAG_NORMALIZE 1
#define FLAG_INVERT 2

#include "common/math/geometry.h"

namespace legit
{

namespace common
{

__LEGIT_EXPORT void printpoint(Point2f p);

__LEGIT_EXPORT void printsize(cv::Size s);

__LEGIT_EXPORT void printsize(Mat& A);

__LEGIT_EXPORT void printrect(cv::Rect r);

__LEGIT_EXPORT void printmat(Mat& A);

__LEGIT_EXPORT void read_csv(Mat& mat, const char* file, char delimiter = ',');

__LEGIT_EXPORT cv::Rect get_roi(Mat& m);

__LEGIT_EXPORT void patch_operation(Mat& target, Mat& patch, cv::Point position, int operation, int origin = ORIGIN_CENTER);

__LEGIT_EXPORT void patch_create(Mat& patch, int width, int height, int type, int flags = 0);

__LEGIT_EXPORT cv::Rect get_mask_bounds(Mat& mat);

__LEGIT_EXPORT const string describe_mat_type(int cvtype);

template <typename T>
void replace_values(Mat& mat, T find, T replace)
{

  for (int j = 0; j < mat.rows; j++)
    {
      T* src = mat.ptr<T>(j);
      for (int i = 0; i < mat.cols; i++)
        if (src[i] == find) src[i] = replace;
    }

}

template <typename T>
string as_string( const T & t )
{
  ostringstream os;
  os << t;
  return os.str();
}

struct __LEGIT_EXPORT LegitException : public std::exception
{
  std::string s;
  LegitException(std::string ss);
  virtual ~LegitException() throw () {}
  const char* what() const throw();
};

}

}

#endif