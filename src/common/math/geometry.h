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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <opencv2/core/core.hpp>
#include "common/math/statistics.h"
#include "common/utils/utils.h"

#ifndef PI
#define PI 3.141592
#endif

#ifndef E
#define E 2.71828182845904523536
#endif

using namespace cv;
using namespace std;

typedef Rect_<float> Rect4f;

// First coordinate is column, the second one is row!
// OpenCV matrix: fist coordinate row, second one column

typedef struct Matrix2f_
{
  float m00;
  float m01;
  float m10;
  float m11;
} Matrix2f;

typedef struct Matrix3f_
{
  float m00;
  float m01;
  float m02;
  float m10;
  float m11;
  float m12;
  float m20;
  float m21;
  float m22;
} Matrix3f;

inline cv::Rect operator+(cv::Rect r, cv::Point p)
{
  r.x += p.x;
  r.y += p.y;
  return r;
}

inline cv::Rect operator-(cv::Rect r, cv::Point p)
{
  r.x -= p.x;
  r.y -= p.y;
  return r;
}

inline cv::Rect operator*(cv::Rect r, float p)
{
  r.x *= p;
  r.y *= p;
  r.width *= p;
  r.height *= p;
  return r;
}

inline cv::Rect operator/(cv::Rect r, float p)
{
  r.x /= p;
  r.y /= p;
  r.width /= p;
  r.height /= p;
  return r;
}

inline Rect4f operator+(Rect4f r, cv::Point2f p)
{
  r.x += p.x;
  r.y += p.y;
  return r;
}

inline Rect4f operator-(Rect4f r, cv::Point2f p)
{
  r.x -= p.x;
  r.y -= p.y;
  return r;
}

inline Matrix2f operator*(Matrix2f m, float factor)
{
  Matrix2f r;

  r.m00 = m.m00 * factor;
  r.m10 = m.m10 * factor;
  r.m01 = m.m01 * factor;
  r.m11 = m.m11 * factor;

  return r;
}

inline Matrix2f operator/(Matrix2f m, float factor)
{
  Matrix2f r;

  r.m00 = m.m00 / factor;
  r.m10 = m.m10 / factor;
  r.m01 = m.m01 / factor;
  r.m11 = m.m11 / factor;

  return r;


}

inline cv::Point2f operator*(cv::Point2f p, Matrix2f m)
{

  cv::Point2f r;

  r.x = m.m00 * p.x + m.m10 * p.y;
  r.y = m.m01 * p.x + m.m11 * p.y;

  return r;
}

inline cv::Point2f operator*(cv::Point2f p, Matrix3f m)
{

  cv::Point2f r;

  r.x = m.m00 * p.x + m.m10 * p.y + m.m20;
  r.y = m.m01 * p.x + m.m11 * p.y + m.m21;

  return r;
}

inline cv::Point operator+=(cv::Point p, cv::Point2f p1)
{

  p.x += (int) p1.x;
  p.y += (int) p1.y;

  return p;

}

inline cv::Point operator-=(cv::Point p, cv::Point2f p1)
{

  p.x -= (int) p1.x;
  p.y -= (int) p1.y;

  return p;
}

inline cv::Point operator+(cv::Point p, cv::Size sz)
{

  p.x += sz.width;
  p.y += sz.height;

  return p;

}

inline cv::Point operator-(cv::Point p, cv::Size sz)
{

  p.x -= sz.width;
  p.y -= sz.height;

  return p;
}

inline cv::Point operator+(cv::Point p, int a)
{

  p.x += a;
  p.y += a;

  return p;
}

inline cv::Point operator+=(cv::Point p, int a)
{

  p.x += a;
  p.y += a;

  return p;
}

inline cv::Point operator-(cv::Point p, int a)
{

  p.x -= a;
  p.y -= a;

  return p;
}

inline cv::Point operator-=(cv::Point p, int a)
{

  p.x -= a;
  p.y -= a;

  return p;
}

inline cv::Point operator/(cv::Point p, int a)
{

  p.x /= a;
  p.y /= a;

  return p;
}

inline cv::Point operator/=(cv::Point p, int a)
{

  p.x /= a;
  p.y /= a;

  return p;
}


Matrix3f operator|(Matrix2f sr, Point2f t);

inline bool operator==(cv::Point p1, cv::Point p2)
{
  return p1.x == p2.x && p1.y == p2.y;
}

inline bool operator==(Point2f p1, Point2f p2)
{
  return p1.x == p2.x && p1.y == p2.y;
}

Mat expand(const Matrix3f m);

Mat expand(const Matrix2f m);

inline bool isnan_matrix(Matrix3f m)
{
  return isnan(m.m00) || isnan(m.m10) || isnan(m.m20) || isnan(m.m01) || isnan(m.m11) || isnan(m.m21) || isnan(m.m02) || isnan(m.m12) || isnan(m.m22);
}

inline bool isnan_matrix(Matrix2f m)
{
  return isnan(m.m00) || isnan(m.m10) || isnan(m.m01) || isnan(m.m11);
}

inline void set_identity(Matrix3f& m)
{
  m.m00 = 1;
  m.m10 = 0;
  m.m20 = 0;
  m.m01 = 0;
  m.m11 = 1;
  m.m21 = 0;
  m.m02 = 0;
  m.m12 = 0;
  m.m22 = 1;
}

inline void set_identity(Matrix2f& m)
{
  m.m00 = 1;
  m.m10 = 0;
  m.m01 = 0;
  m.m11 = 1;
}

float determinant(Matrix2f m);

Matrix2f invert(Matrix2f m);

Matrix2f extract_rotation_scale(Matrix3f m);

Point2f extract_translation(Matrix3f m);

Matrix3f transpose(Matrix3f m);

Matrix2f transpose(Matrix2f m);

Matrix3f simple_affine_transformation(float tx, float ty, float r, float sx, float sy);

Matrix3f compute_affine_transformation(vector<Point2f> from, vector<Point2f> to);

Matrix3f compute_affine_transformation(vector<Point2f> from, vector<Point2f> to, vector<float> weights);

Rect4f compute_bounds(Rect4f bounds, vector<Point2f> from, vector<Point2f> to, vector<float> weights);

Point2f transform_point(Point2f in, Matrix3f& t);

Point2f transform_point(Point2f in, Matrix3f& t, Point2f relative);

cv::Rect transform_rect(cv::Rect in, Matrix3f& t);

Matrix distances(Point2f* points, int size);

void distances(Point2f origin, Point2f* points, int size, float* result);

void print(Matrix3f t);

void print(Matrix2f t);

cv::Rect intersection(cv::Rect r1, cv::Rect r2);

Rect4f combine(Rect4f r1, Rect4f r2, float factor);

cv::Rect expand(cv::Rect r, int margin);

cv::Rect scale(cv::Rect r, float scale);

cv::Rect resize(cv::Rect r, int width, int height);

void decompose_affine_transformation(Matrix3f M, Point2f* translation, float* rotation, float* scaleX, float* scaleY);

void expand(Point2f* points, int length, Point2f origin, float margin);

float distance(Point2f p);

#endif