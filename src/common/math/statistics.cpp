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
#include <stdio.h>
#include <cmath>

//#include "math.h"
#include "common/math/statistics.h"
#include "common/utils/utils.h"
#include "common/utils/debug.h"

using namespace legit;

void white_noise_image(Mat& image)
{

  if (image.depth() == CV_8U)
    {

      if (image.channels() > 1)
        {

          for (int j = 0; j < image.rows; j++)
            {
              uchar* data = image.ptr<uchar>(j);
              for (int i = 0; i < image.cols; i++)
                {
                  for (int c = 0; c < image.channels(); c++)
                    data[i*image.channels() + c] = saturate_cast<uchar>(CLAMP3(0, 255, (int)(RANDOM_UNIFORM * 255)));
                }
            }


        }
      else
        {

          for (int j = 0; j < image.rows; j++)
            {
              uchar* data = image.ptr<uchar>(j);
              for (int i = 0; i < image.cols; i++)
                {
                  data[i] = saturate_cast<uchar>(CLAMP3(0, 255, (int)(RANDOM_UNIFORM * 255)));
                }
            }

        }

      return;
    }


  if (image.depth() == CV_32F)
    {

      if (image.channels() > 1)
        {


          for (int j = 0; j < image.rows; j++)
            {
              float* data = image.ptr<float>(j);
              for (int i = 0; i < image.cols; i++)
                {
                  for (int c = 0; c < image.channels(); c++)
                    data[i*image.channels() + c] = saturate_cast<float>(RANDOM_UNIFORM);
                }
            }

        }
      else
        {

          for (int j = 0; j < image.rows; j++)
            {
              float* data = image.ptr<float>(j);
              for (int i = 0; i < image.cols; i++)
                {
                  data[i] = saturate_cast<float>(RANDOM_UNIFORM);
                }
            }
        }

      return;
    }

}


// SAMPLE_GAUSSIAN Draw N random row vectors from a Gaussian distribution
// samples = sample_gaussian(mean, cov, N)
void sample_gaussian(Matrix& mu, Matrix& sigma, int N, Matrix& out, int offset)
{

  // If Y = CX, Var(Y) = C Var(X) C'.
  // So if Var(X)=I, and we want Var(Y)=Sigma, we need to find C. s.t. Sigma = C C'.
  // Since Sigma is psd, we have Sigma = U D U' = (U D^0.5) (D'^0.5 U').

  int n = mu.cols;
  SVD svd(sigma);

  Mat_<double> m = Mat_<double>(n, N);

  for (int i = 0; i < m.rows; i++)
    {
      for (int j = 0; j < m.cols; j++)
        {
          m(i, j) = randn();
        }
    }

  sqrt(svd.w, svd.w);

  Mat v = (svd.u * Mat::diag(svd.w)) * m;
  for (int j = 0; j < v.cols; j++)
    {
      out.row(j + offset) = v.col(j).t() + mu;
    }
}

/*static double u_direct_static[4];
static double w_direct_static[2];
*/
void sample_gaussian2(Matrix& mu, Matrix& sigma, int N, Matrix& out, int offset)
{

  // If Y = CX, Var(Y) = C Var(X) C'.
  // So if Var(X)=I, and we want Var(Y)=Sigma, we need to find C. s.t. Sigma = C C'.
  // Since Sigma is psd, we have Sigma = U D U' = (U D^0.5) (D'^0.5 U').

  double* u_direct;
  double* w_direct;

  int n = mu.cols;

  /* if (n == 2) {

       double* sigma_direct = (double *) sigma.data;

       u_direct = u_direct_static;
       w_direct = w_direct_static;

       float t1 = sigma_direct[0] + sigma_direct[3];
       float t2 = SQRT(sigma_direct[0]*sigma_direct[0] - 2.0*sigma_direct[0]*sigma_direct[3]
                   + 4*sigma_direct[1]*sigma_direct[1] + sigma_direct[3]*sigma_direct[3]) ;

       w_direct[0] = (t1 + t2)/2.0 ;
       w_direct[1] = (t1 - t2)/2.0 ;

       float x12 = -(sigma_direct[0] - w_direct[0]) / sigma_direct[1] ;
       float xs = SQRT(1.0 + x12*x12) ;

       u_direct[0] = 1.0 / xs ;
       u_direct[2] = x12 / xs ;

       if (u_direct[0] * u_direct[2] < 0) {
           u_direct[1] = -u_direct[2];
           u_direct[3] = u_direct[0];
       } else {
           u_direct[1] = -u_direct[2];
           u_direct[3] = u_direct[0];
       }

       w_direct[0] = SQRT(w_direct[0]);
       w_direct[1] = SQRT(w_direct[1]);

   } else {*/
  SVD svd(sigma);

  sqrt(svd.w, svd.w);

  u_direct = (double *) svd.u.data;
  w_direct = (double *) svd.w.data;

  //}

  double* mu_direct = (double *) mu.data;

  for (int r = 0; r < N; r++)
    {

      double* out_direct = (double *)out.ptr(r + offset);
      for (int j = 0; j < n; j++)
        {

          double sum = 0;

          for (int i = 0; i < n; i++)
            {
              sum += u_direct[j*n + i] * w_direct[i] * randn(); // RANDOM_NORMAL;
            }

          out_direct[j] = sum + mu_direct[j];
        }

    }


  /*
      Mat v = (svd.u * Mat::diag(svd.w)) * m;
      for (int j = 0; j < v.cols; j++) {
          out.row(j + offset) = v.col(j).t() + mu;
      }*/
}


// SAMPLE_MAP samples points from a probability map
// map must be a normalized 1 channel CV_32F image!!!
void sample_map(Mat& map, Point* points, int count, float* values)
{

  int nc = map.cols;
  int nr = map.rows;

  float* cols = new float[nc];
  float* rows = new float[nr];

  rows[0] = sum(map.row(0))[0];
  for (int i = 1; i < nr; i++)
    {
      rows[i] = rows[i-1] + sum(map.row(i))[0];
    }

  if (rows[nr-1] == 0)
    throw exception();

  for (int p = 0; p < count; p++)
    {
      float r = RANDOM_UNIFORM;
      int y = 0;
      for (; y < nr; y++)
        {
          if (rows[y] > r) break;
        }
      if (y == nr)
        {
          points[p].x = -1;
          points[p].y = -1;
          if (values) values[p] = 0;
          break;
        }

      r = RANDOM_UNIFORM;
      Mat row = map.row(y);
      float rs = sum(row)[0];
      cols[0] = row.at<float>(0, 0) / rs;
      for (int i = 1; i < nc; i++)
        {
          cols[i] = cols[i-1] + row.at<float>(0, i) / rs;
        }
      int x = 0;
      for (; x < nc; x++)
        {
          if (cols[x] > r)
            break;
        }

      points[p].x = y == nr ? -1 : x;
      points[p].y = y == nr ? -1 : y;
      if (values) values[p] = y == nr ? 0 : map.at<float>(y, x);
    }

  delete [] cols;
  delete [] rows;
}

void random_permutation(int N, int* sequence, float* weights)
{

  float* nweights = new float[N];

  if (weights == NULL)
    {
      for (int i = 0; i < N; i++) nweights[i] = 1.0f;
    }
  else
    {
      for (int i = 0; i < N; i++) nweights[i] = weights[i];
    }

  for (int j = 0; j < N; j++)
    {

      float sum = 0;
      for (int i = 0; i < N; i++) sum += nweights[i];


      if (sum == 0)
        {
          // That means that we have no choices left ... so just set remaining slots to -1
          for (int i = j; i < N; i++)
            sequence[i] = -1;
          break;
        }

      float r = RANDOM_UNIFORM;

      float val = 0;
      for (int i = 0; i < N; i++)
        {
          val += nweights[i] / sum;
          if (val >= r)
            {
              sequence[j] = i;
              nweights[i] = 0;
              break;
            }
        }

    }

  delete [] nweights;
}

vector<int> random_permutation(int N)
{

  int* indices = new int[N];

  for (int i = 0; i < N; i++) indices[i] = i;

  random_permutation(N, indices);

  vector<int> result;

  for (int i = 0; i < N; i++) result.push_back(indices[i]);

  delete [] indices;

  return result;

}

int compare_double(const void *a, const void *b)
{
  double c = (*(double*)a-*(double*)b);
  return (c < 0) ? -1 : (c > 0) ? 1 : 0;
}

double median(Matrix& m)
{

  int size = m.rows * m.cols;
  double* d = new double[size];

  int k = 0;
  for (int i = 0; i < m.rows; i++)
    {
      for (int j = 0; j < m.cols; j++)
        {
          d[k++] = m(i, j);
        }
    }

  qsort(d, m.rows * m.cols, sizeof(double), compare_double);

  int middle = size/2;
  double median = 0;

  if (size%2==0)
    median = (d[middle-1]+d[middle])/2;
  else
    median = (d[middle]);

  delete [] d;

  return median;
}

double median(vector<double>& m)
{

  int size = m.size();
  double* d = new double[size];

  for (int i = 0; i < size; i++)
    {
      d[i] = m[i];
    }

  qsort(d, size, sizeof(double), compare_double);

  int middle = size/2;
  double median = 0;

  if (size%2==0)
    median = (d[middle-1]+d[middle])/2;
  else
    median = (d[middle]);

  free(d);

  return median;
}

double quantile(vector<double>& m, int q, int r)
{

  r = MIN(q-1, MAX(0, r));

  int size = m.size();
  int i;
  double* d = new double[size];

  for (i = 0; i < size; i++)
    {
      d[i] = m[i];
    }

  qsort(d, size, sizeof(double), compare_double);

  i = round(((double)size*(r+1))/(q)) - 1;
  //i = floor(pos);
  //double k = pos - i;
  double quantile = 0;

  //DEBUGMSG("%f %f %d\n", pos, k, i);

  /* if (k != 0)
       quantile = (d[i-1] * (k) + d[i] * (1-k));
   else */
  quantile = (d[i]);

  free(d);

  return quantile;
}

Matrix row_weighted_mean(Matrix& mtr, Matrix& weights)
{

  int nc = mtr.cols;
  int nr = mtr.rows;

  Matrix result(1, nc);

  float wsum = 0;

  for (int i = 0; i < nr; i++)
    {
      wsum += weights(i, 0);
    }
  //printf("%f \n", wsum);
  //printmat(result);
  for (int i = 0; i < nc; i++)
    {
      result(0, i) = mtr.col(i).dot(weights) / wsum;
    }

  return result;
}

Matrix row_weighted_covariance(Matrix& mtr, Matrix& weights)
{

  Matrix mx = mtr.clone();

  Matrix mean = row_weighted_mean(mtr, weights);

  int nc = mtr.cols;
  int nr = mtr.rows;

  for (int i = 0; i < nr; i++)
    {
      mx.row(i) -= mean;
    }

  float wsum = 0;
  for (int i = 0; i < nr; i++)
    {
      wsum += weights(i, 0);
    }

  float factor = 0;

  for (int i = 0; i < nr; i++)
    {
      float f = weights(i, 0) / wsum;
      factor += f * f;
    }

  factor = 1 / (1 - factor);
  Matrix w = (weights * Mat::ones(1, nc, mx.type()));
  Matrix result = (mx.t() * mx.mul(w, 1 / wsum *  factor));

  return result;

}

void threshold(Mat& mat, float threshold)
{

  for (int i = 0; i < mat.rows; i++)
    {
      float* data = (float *) mat.ptr(i);
      for (int j = 0; j < mat.cols; j++)
        {
          data[j] = (data[j] > threshold) ? 1 : 0;
        }
    }

}

void high_pass(Mat& mat, float threshold)
{

  for (int i = 0; i < mat.rows; i++)
    {
      float* data = (float *) mat.ptr(i);
      for (int j = 0; j < mat.cols; j++)
        {
          if (data[j] < threshold)
            data[j] = 0;
        }
    }

}

void low_pass(Mat& mat, float threshold)
{

  for (int i = 0; i < mat.rows; i++)
    {
      float* data = (float *) mat.ptr(i);
      for (int j = 0; j < mat.cols; j++)
        {
          if (data[j] > threshold)
            data[j] = 0;
        }
    }

}
