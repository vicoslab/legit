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

#include "common/math/geometry.h"
#include "common/utils/utils.h"
#include "common/utils/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/imgproc/imgproc.hpp>

using namespace legit;
using namespace legit::common;

Matrix3f operator|(Matrix2f sr, Point2f t)
{

  Matrix3f r;

  r.m00 = sr.m00;
  r.m10 = sr.m10;
  r.m01 = sr.m01;
  r.m11 = sr.m11;

  r.m02 = 0;
  r.m12 = 0;
  r.m22 = 1;

  r.m20 = t.x;
  r.m21 = t.y;

  return r;

}

Mat expand(const Matrix3f m)
{

  Mat mm(3, 3, CV_32F);

  mm.at<float>(0, 0) = m.m00;
  mm.at<float>(1, 0) = m.m01;
  mm.at<float>(2, 0) = m.m02;
  mm.at<float>(0, 1) = m.m10;
  mm.at<float>(1, 1) = m.m11;
  mm.at<float>(2, 1) = m.m12;
  mm.at<float>(0, 2) = m.m20;
  mm.at<float>(1, 2) = m.m21;
  mm.at<float>(2, 2) = m.m22;

  return mm;
}

Mat expand(const Matrix2f m)
{

  Mat mm(2, 2, CV_32F);

  mm.at<float>(0, 0) = m.m00;
  mm.at<float>(0, 1) = m.m01;
  mm.at<float>(0, 1) = m.m10;
  mm.at<float>(1, 1) = m.m11;

  return mm;
}

Matrix2f extract_rotation_scale(Matrix3f m)
{

  Matrix2f r;

  r.m00 = m.m00;
  r.m10 = m.m10;
  r.m01 = m.m01;
  r.m11 = m.m11;

  return r;

}

Point2f extract_translation(Matrix3f m)
{

  Point2f r;

  r.x = m.m20;
  r.y = m.m21;

  return r;

}

Matrix3f transpose(Matrix3f m)
{

  Matrix3f r;

  r.m00 = m.m00;
  r.m10 = m.m01;
  r.m20 = m.m02;

  r.m01 = m.m10;
  r.m11 = m.m11;
  r.m21 = m.m12;

  r.m02 = m.m20;
  r.m12 = m.m21;
  r.m22 = m.m22;

  return r;

}

Matrix2f transpose(Matrix2f m)
{

  Matrix2f r;

  r.m00 = m.m00;
  r.m10 = m.m01;
  r.m01 = m.m10;
  r.m11 = m.m11;

  return r;

}

float determinant(Matrix2f m)
{
  return m.m00 * m.m11 - m.m10 * m.m01;
}

Matrix2f invert(Matrix2f m)
{

  Matrix2f r;
  float det = determinant(m);

  r.m00 = m.m11 / det;
  r.m11 = m.m00 / det;
  r.m10 = -m.m01 / det;
  r.m01 = -m.m10 / det;

  return r;

}


Matrix3f simple_affine_transformation(float tx, float ty, float r, float sx, float sy)
{

  Matrix3f m;

  m.m00 = cos(r) * sx; // mislim, da je napaka v konstrukciji matrike za afino deformacijo -- ta koda konstruira transponirano matriko
  m.m10 = -sin(r) * sy;
  m.m20 = tx;
  m.m01 = sin(r) * sx;
  m.m11 = cos(r) * sy;
  m.m21 = ty;
  m.m02 = 0; // zares zadnje dimenzije ne rabimo za Afino deformacijo, ker slednja ne vpliva na konèno toèko
  m.m12 = 0;
  m.m22 = 1;
  // popravi tudi transform_point(), ker je narobe napisana in predvideva transponirano transformacijsko matriko
  return m;
}

Matrix3f compute_affine_transformation(vector<Point2f> from, vector<Point2f> to)
{

  if (from.size() != to.size())
    throw LegitException("Unable to compute affine transform: from/to point set not the same size.");

  if (from.size() == 3)
    {
      Point2f afrom[3];
      Point2f ato[3];

      afrom[0] = from[0];
      afrom[1] = from[1];
      afrom[2] = from[2];
      ato[0] = to[0];
      ato[1] = to[1];
      ato[2] = to[2];

      Mat tr;

      tr = getAffineTransform(afrom, ato);

      Matrix3f result;

      result.m00 = tr.at<double>(0, 0);
      result.m10 = tr.at<double>(0, 1);
      result.m20 = tr.at<double>(0, 2);

      result.m01 = tr.at<double>(1, 0);
      result.m11 = tr.at<double>(1, 1);
      result.m21 = tr.at<double>(1, 2);

      result.m02 = 0;
      result.m12 = 0;
      result.m22 = 1;

      return result;

    }
  else
    {
      Mat_<float> A = Mat_<float>(from.size() * 2, 6);
      Mat_<float> L = Mat_<float>(from.size() * 2, 1);

      A.setTo(0);

      for (size_t i = 0; i < from.size(); i++)
        {
          A(i*2, 0) = from[i].x;
          A(i*2, 1) = from[i].y;
          A(i*2, 2) = 1;
          A(i*2+1, 3) = from[i].x;
          A(i*2+1, 4) = from[i].y;
          A(i*2+1, 5) = 1;
          L(i*2, 0) = to[i].x;
          L(i*2+1, 0) = to[i].y;
        }

      Mat inv;

      invert(A.t() * A, inv);

      Mat X = inv * A.t() * L;

      Matrix3f result;

      result.m00 = X.at<float>(0, 0);
      result.m10 = X.at<float>(1, 0);
      result.m20 = X.at<float>(2, 0);

      result.m01 = X.at<float>(3, 0);
      result.m11 = X.at<float>(4, 0);
      result.m21 = X.at<float>(5, 0);

      result.m02 = 0;
      result.m12 = 0;
      result.m22 = 1;

      return result;
    }


}

Matrix3f compute_affine_transformation(vector<Point2f> from, vector<Point2f> to, vector<float> weights)
{
  if (from.size() != to.size() || from.size() != weights.size())
    throw LegitException("Unable to compute affine transform: from/to point set not the same size.");

  Mat_<float> A = Mat_<float>(from.size() * 2, 6);
  Mat_<float> L = Mat_<float>(from.size() * 2, 1);

  A.setTo(0);

  float weights_total = 0;

  for (size_t i = 0; i < from.size(); i++)
    {
      weights_total += weights[i];
    }

  for (size_t i = 0; i < from.size(); i++)
    {
      float w = sqrt(weights[i] / (2 * weights_total));

      A(i*2, 0) = from[i].x * w;
      A(i*2, 1) = from[i].y * w;
      A(i*2, 2) = w;
      A(i*2+1, 3) = from[i].x * w;
      A(i*2+1, 4) = from[i].y * w;
      A(i*2+1, 5) = w;
      L(i*2, 0) = to[i].x * w;
      L(i*2+1, 0) = to[i].y * w;
    }

  // TODO: optimize
  Mat inv;
  invert(A.t() * A, inv);
  Mat X = inv * A.t() * L;

  Matrix3f result;
  result.m00 = X.at<float>(0, 0);
  result.m10 = X.at<float>(1, 0);
  result.m20 = X.at<float>(2, 0);

  result.m01 = X.at<float>(3, 0);
  result.m11 = X.at<float>(4, 0);
  result.m21 = X.at<float>(5, 0);

  result.m02 = 0;
  result.m12 = 0;
  result.m22 = 1;

  return result;

}

Rect4f compute_bounds(Rect4f bounds, vector<Point2f> from, vector<Point2f> to, vector<float> weights)
{
  if (from.size() != to.size() || from.size() != weights.size())
    throw LegitException("Unable to compute: from/to point set not the same size.");

  Mat_<float> A = Mat_<float>(from.size() * 2, 4);
  Mat_<float> L = Mat_<float>(from.size() * 2, 1);

  A.setTo(0);

  float weights_total = 0;

  for (size_t i = 0; i < from.size(); i++)
    {
      weights_total += weights[i];
    }

  for (size_t i = 0; i < from.size(); i++)
    {
      float w = sqrt(weights[i] / (2 * weights_total));

      A(i*2, 0) = to[i].x * w;
      A(i*2, 1) = -w;
      A(i*2+1, 2) = to[i].y * w;
      A(i*2+1, 3) = -w;
      L(i*2, 0) = ((from[i].x - bounds.x) / bounds.width) * w;
      L(i*2+1, 0) = ((from[i].y - bounds.y) / bounds.height) * w;
    }

  Mat inv;

  invert(A.t() * A, inv);

  Mat X = inv * A.t() * L;

  Rect4f result;

  result.width = 1 / X.at<float>(0, 0);
  result.height = 1 / X.at<float>(2, 0);

  result.x = X.at<float>(1, 0) * result.width;
  result.y = X.at<float>(3, 0) * result.height;

  return result;

}

Rect4f combine(Rect4f r1, Rect4f r2, float factor)
{
  Rect4f result;

  factor = MIN(1.0f, MAX(0.0f, factor));

  result.x = (factor) * r1.x + (1-factor) * r2.x;
  result.y = (factor) * r1.y + (1-factor) * r2.y;
  result.width = (factor) * r1.width + (1-factor) * r2.width;
  result.height = (factor) * r1.height + (1-factor) * r2.height;

  return result;
}

Point2f transform_point(Point2f in, Matrix3f& t)   // narobe napisana transformacija, ker predvideva transponirano matriko
{

  Point2f out;

  out.x = t.m00 * in.x + t.m10 * in.y + t.m20;
  out.y = t.m01 * in.x + t.m11 * in.y + t.m21;

  return out;
}

Point2f transform_point(Point2f in, Matrix3f& t, Point2f relative)
{

  Point2f out;

  in.x -= relative.x;
  in.y -= relative.y;

  out.x = t.m00 * in.x + t.m10 * in.y + t.m20;
  out.y = t.m01 * in.x + t.m11 * in.y + t.m21;

  out.x += relative.x;
  out.y += relative.y;

  return out;

}

Rect transform_rect(Rect in, Matrix3f& t)
{

  Rect result;

  Point2f p1 = transform_point(Point2f(in.x, in.y), t);
  Point2f p2 = transform_point(Point2f(in.x + in.width, in.y), t);
  Point2f p3 = transform_point(Point2f(in.x, in.y + in.height), t);
  Point2f p4 = transform_point(Point2f(in.x + in.width, in.y + in.height), t);

  result.x = (int) MIN(MIN(MIN(p1.x, p2.x), p3.x), p4.x);
  result.y = (int) MIN(MIN(MIN(p1.y, p2.y), p3.y), p4.y);
  result.width = (int) MAX(MAX(MAX(p1.x, p2.x), p3.x), p4.x) - result.x;
  result.height = (int) MAX(MAX(MAX(p1.y, p2.y), p3.y), p4.y) - result.y;

  return result;
}

void print(Matrix3f t)
{
  printf("\n");
  printf("%.3f %.3f %.3f \n", t.m00, t.m10, t.m20);
  printf("%.3f %.3f %.3f \n", t.m01, t.m11, t.m21);
  printf("%.3f %.3f %.3f \n", t.m02, t.m12, t.m22);
  printf("\n");
}

void print(Matrix2f t)
{
  printf("\n");
  printf("%.3f %.3f \n", t.m00, t.m10);
  printf("%.3f %.3f \n", t.m01, t.m11);
  printf("\n");
}

Rect intersection(Rect r1, Rect r2)
{
  Rect r;

  r.x = MAX(r1.x, r2.x);
  r.y = MAX(r1.y, r2.y);
  r.width = MIN(r1.width + r1.x, r2.width + r2.x) - r.x;
  r.height = MIN(r1.height + r1.y, r2.height + r2.y) - r.y;

  return r;
}

Rect expand(Rect r, int margin)
{
  Rect result;

  result.x = MIN(r.x + r.width / 2, r.x - margin);
  result.y = MIN(r.y + r.height / 2, r.y - margin);
  result.width = MAX(0, r.width + 2 * margin);
  result.height = MAX(0, r.height + 2 * margin);

  return result;
}

Rect scale(Rect r, float scale)
{

  Rect result;

  int cx = r.x + r.width / 2;
  int cy = r.y + r.height / 2;

  result.x = (int) (cx - r.width * scale / 2);
  result.y = (int) (cy - r.height * scale / 2);
  result.width = (int) (r.width * scale);
  result.height = (int) (r.height * scale);

  return result;

}

void expand(Point2f* points, int length, Point2f origin, float margin)
{

  for (int i = 0; i < length; i++)
    {
      Point2f vector = points[i] - origin;

      float len = sqrt(vector.x * vector.x + vector.y * vector.y);
      float scale = (len + margin) / len;

      points[i] = origin + vector * scale;
    }

}

Rect resize(Rect r, int width, int height)
{
  Rect result;

  result.x = (r.x + r.width / 2) - width / 2;
  result.y = (r.y + r.height / 2) - height / 2;
  result.width = width;
  result.height = height;

  return result;

}

void decompose_affine_transformation(Matrix3f M, Point2f* translation, float* rot, float* scaleX, float* scaleY)
{

  Matrix3f Mt = transpose(M);

  Matrix2f rotsc = extract_rotation_scale(Mt);

  *translation = Point2f(M.m20, M.m21);

  float sX = sqrt(M.m00 * M.m00 + M.m01 * M.m01);
  float sY = sqrt(M.m10 * M.m10 + M.m11 * M.m11);

  if (determinant(rotsc) < 0)
    {
      sX = -sX;
      rotsc.m00 = -rotsc.m00;
      rotsc.m10 = -rotsc.m10;
    }

  rotsc.m00 /= sX;
  rotsc.m10 /= sX;
  rotsc.m01 /= sY;
  rotsc.m11 /= sY;


  float rotationAngle = atan2(rotsc.m01, rotsc.m00);
  /*
  	double rotationAngle = sqrt(2.0 + rotsc.m00 + rotsc.m11) / 2.0;

  	double w4 = (4.0 * rotationAngle);
  	double rotation = (rotsc.m10 - rotsc.m01) / w4;

  	double tr = rotsc.m00 + rotsc.m11 + 1;

  	if (tr > 0) {
  		double S = sqrt(tr + 1.0) * 2; // S=4*qw
  		rotationAngle = 0.25 * S;
  		rotation = (rotsc.m10 - rotsc.m01) / S;
  	} else if ((rotsc.m00 > rotsc.m11) & (rotsc.m00 > 1)) {
  		double S = sqrt(rotsc.m00 - rotsc.m11) * 2; // S=4*qx
  		rotationAngle = 0;
  		//rotationX = 0.25 * S;
  		**rotationY = (rotsc.m01 + rotsc.m10) / S;
  		rotation = 0;
  	} else if (rotsc.m11 > 1) {
  		double S = sqrt(rotsc.m11 - rotsc.m00) * 2; // S=4*qy
  		rotationAngle = 0;
  		rotation = 0;
  	} else {
  		double S = sqrt(1.0 + 1 - rotsc.m00 - rotsc.m11) * 2; // S=4*qz
  		rotationAngle = (rotsc.m10 - rotsc.m01) / S;
  		rotation = 0.25 * S;
  	}
  */

  *rot = rotationAngle;

  *scaleX = sX;
  *scaleY = sY;
//    *rotation = atan2(M.m01 / *scaleX, M.m00 / *scaleX);


  DEBUGMSG("Translation %f,%f, rotation %f, scale %f, %f \n", (*translation).x, (*translation).y, *rot, *scaleX, *scaleY);

}

Matrix distances(Point2f* points, int size)
{

  Mat_<double> d = Mat_<double>(size, size);

  for (int i = 0; i < size; i++)
    {
      d(i, i) = 0;
      for (int j = i+1; j < size; j++)
        {
          float dx = points[i].x - points[j].x;
          float dy = points[i].y - points[j].y;
          d(i, j) = sqrt(dx * dx + dy * dy);
          d(j, i) = d(i, j);
        }
    }

  return d;
}

void distances(Point2f origin, Point2f* points, int size, float* result)
{

  for (int i = 0; i < size; i++)
    {
      float dx = points[i].x - origin.x;
      float dy = points[i].y - origin.y;
      result[i] = sqrt(dx * dx + dy * dy);
    }

}

float distance(Point2f p)
{
  return sqrt(p.x * p.x + p.y * p.y);
}
