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

#ifndef STATISTICS_H
#define STATISTICS_H

#include "common/math/math.h"
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

typedef Mat_<double> Matrix;

inline double randn(double mu=0.0, double sigma=1.0)
{
  static bool deviateAvailable = false;
  static double storedDeviate; //deviate from previous calculation
  double polar, rsquared, var1, var2;

  //	If no deviate has been stored, the polar Box-Muller transformation is
  //	performed, producing two independent normally-distributed random
  //	deviates.  One is stored for the next round, and one is returned.
  if (!deviateAvailable)
    {

      //	choose pairs of uniformly distributed deviates, discarding those
      //	that don't fall within the unit circle
      do
        {
          var1=2.0*( RANDOM_UNIFORM ) - 1.0;
          var2=2.0*( RANDOM_UNIFORM ) - 1.0;
          rsquared=var1*var1+var2*var2;
        }
      while ( rsquared>=1.0 || rsquared == 0.0);

      //	calculate polar tranformation for each deviate
      polar=sqrt(-2.0*log(rsquared)/rsquared);

      //	store first deviate and set flag
      storedDeviate=var1*polar;
      deviateAvailable=true;

      //	return second deviate
      return var2*polar*sigma + mu;
    }

  //	If a deviate is available from a previous call to this function, it is
  //	returned, and the flag is set to false.
  else
    {
      deviateAvailable=false;
      return storedDeviate*sigma + mu;
    }
}

void white_noise_image(Mat& image);

/**
 Draw N random row vectors from a Gaussian distribution

 samples = sample_gaussian(mean, cov, N)
*/
void sample_gaussian(Matrix& mu, Matrix& sigma, int N, Matrix& out, int offset);

/**
 Draw N random row vectors from a Gaussian distribution

 samples = sample_gaussian(mean, cov, N)
 This version tries to be as fast as possible

*/
void sample_gaussian2(Matrix& mu, Matrix& sigma, int N, Matrix& out, int offset);

void sample_map(Mat& map, cv::Point* points, int count, float* values = NULL);


void random_permutation(int N, int* sequence, float* weights = NULL);

vector<int> random_permutation(int N);

double median(Matrix& m);

double median(vector<double>& m);

double quantile(vector<double>& m, int q, int r);

Matrix row_weighted_mean(Matrix& mtr, Matrix& weights);

Matrix row_weighted_covariance(Matrix& mtr, Matrix& weights);

void threshold(Mat& mat, float threshold);

void high_pass(Mat& mat, float threshold);

void low_pass(Mat& mat, float threshold);

#endif