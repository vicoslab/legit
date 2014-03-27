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

#include "crossentropy.h"
#include "common/gui/gui.h"
#include "common/utils/defs.h"

namespace legit
{

namespace tracker
{

typedef struct PatchCostPair_
{
  float cost;
  int index;
} PatchCostPair;

int compare_patch_cost_pair (const void* i, const void* j)
{
  float c = ( ((PatchCostPair*)j)->cost - ((PatchCostPair*)i)->cost );
  return (c < 0) ? -1 : (c > 0) ? 1 : 0;
}

typedef struct
{
  int index;
  float weight;
} NeighbourConstraint;

void cross_entropy_global_move(Image& image, PatchSet& patches, Matrix& mean, Matrix& covariance, CrossEntropyParameters params, OptimizationStatus& status)
{

  status.reset();

  Matrix globalC = covariance;
  Matrix globalM = mean;

  Point2f center = patches.mean_position();

  double gamma_low = 0;
  double gamma_high = 0;

  PatchCostPair* sampled_positions = new PatchCostPair[params.max_samples];

  Mat_<double> global_samples = Mat_<double>(params.max_samples, 2); // this matrix stores global CE parameters
  Mat_<double> global_elite_samples = Mat_<double>(params.elite_samples, 2); // this matrix stores elite global CE parameters
  Mat_<double> global_elite_weights = Mat_<double>(params.elite_samples, 1); // this vector stores elite global CE weighths / responses

  Rect4f region = patches.region();
//    region.x = -region.width / 2;
//    region.y = -region.height / 2;

  // Number of global cross entropy iterations ...
  int i;
  for (i = 0; i < params.iterations; i++)
    {

      int count = patches.size();

      int samples_count = 0;
      sample_gaussian2(globalM, globalC, params.min_samples, global_samples, samples_count);


      for (int k = 0; k < params.min_samples; k++)
        {
          Point2f A(global_samples.at<double>(k, 0), global_samples.at<double>(k, 1));

          PatchCostPair response;
          response.index = k;
          response.cost = 0.0;

          for (int j = 0; j < patches.size(); j++)
            {
              Point2f tp = patches.get_position(j) + A;
              float cost = exp(- patches.response(image, j, tp) );
              response.cost += cost * patches.get_weight(j);
            }
          sampled_positions[k] = response;
        }

      samples_count = params.min_samples;

      // TODO: not all samples are needed : create a dynamic list of N elite samples data structure
      qsort(sampled_positions, samples_count, sizeof(PatchCostPair), compare_patch_cost_pair);

      while (samples_count < params.max_samples)
        {
          // TODO: verify in CE algorithm (pg. 191)
          if (gamma_low < sampled_positions[params.elite_samples].cost || gamma_high < sampled_positions[0].cost)
            {
              gamma_low = sampled_positions[params.elite_samples].cost;
              gamma_high = sampled_positions[1].cost;
              break;
            }

          sample_gaussian2(globalM, globalC, params.add_samples, global_samples, samples_count);

          for (int k = samples_count; k < samples_count + params.add_samples; k++)
            {
              Point2f A(global_samples.at<double>(k, 0), global_samples.at<double>(k, 1));
              PatchCostPair response;
              response.index = k;
              response.cost = 0.0;

              for (int j = 0; j < patches.size(); j++)
                {
                  Point2f tp = patches.get_position(j) + A;
                  float cost = exp(-  patches.response(image, j, tp)  );
                  response.cost +=  cost * patches.get_weight(j);
                }

              sampled_positions[k] = response;
            }
          samples_count += params.add_samples;

          qsort(sampled_positions, samples_count, sizeof(PatchCostPair), compare_patch_cost_pair);

        }

      for (int j = 0; j < params.elite_samples; j++)
        {
          Mat r = global_elite_samples.row(j);
          global_samples.row(sampled_positions[j].index).copyTo(r);
          global_elite_weights(j, 0) = sampled_positions[j].cost;
        }

      // TODO: fail?
      if (global_elite_weights(0, 0) == 0) global_elite_weights.setTo(1);

      globalC = row_weighted_covariance(global_elite_samples, global_elite_weights);

      globalM = row_weighted_mean(global_elite_samples, global_elite_weights);

      double det = globalC.at<double>(0, 0) * globalC.at<double>(1, 1) - globalC.at<double>(1, 0) * globalC.at<double>(0, 1);

      if (det < params.terminate || samples_count >= params.max_samples)
        {
          for (int j = 0; j < patches.size(); j++)
            {
              status.converged(j, i);
//                status[j].iterations = i;
//                status[j].flags |= OPTIMIZATION_CONVERGED;
            }

          break;
        }
    }

  Point2f A(globalM.at<double>(0), globalM.at<double>(1));

  for (int j = 0; j < patches.size(); j++)
    {
      // TODO: optimize
      /*        status[j].position = patches.get_position(j) + A;
              status[j].value = exp(- patches.response(image, j, status[j].position));*/
      Point2f p = patches.get_position(j) + A;
      status.set(j, p, exp(- patches.response(image, j, p)));
    }

  delete [] sampled_positions;

  if (i < params.iterations)
    {
      DEBUGMSG("Global iterations: %d\n", i);
    }
  else
    {
      DEBUGMSG("Warning: global optimization did not converge! \n");
    }

}

void cross_entropy_global_affine(Image& image, PatchSet& patches, Matrix& mean, Matrix& covariance, CrossEntropyParameters params, SizeConstraints size_constraints, OptimizationStatus& status)
{

  status.reset();

  /*    for (int i = 0; i < patches.size(); i++) {
          status[i].id = patches.get_id(i);
          status[i].flags = 0;
      }*/

  Matrix globalC = covariance;
  Matrix globalM = mean;

  Point2f center = patches.mean_position();

  double gamma_low = 0;
  double gamma_high = 0;

  PatchCostPair* sampled_positions = new PatchCostPair[params.max_samples];

  Mat_<double> global_samples = Mat_<double>(params.max_samples, 5); // this matrix stores global CE parameters
  Mat_<double> global_elite_samples = Mat_<double>(params.elite_samples, 5); // this matrix stores elite global CE parameters
  Mat_<double> global_elite_weights = Mat_<double>(params.elite_samples, 1); // this vector stores elite global CE weighths / responses

  Rect4f region = patches.region();
  region.x = -region.width / 2;
  region.y = -region.height / 2;

#ifdef BUILD_DEBUG
  Canvas* debug = get_canvas("optimization");
#endif

  /*
  	float sx_min, sx_max, sy_min, sy_max ;

  	sx_min = (float)(size_constraints.min_size.width) / region.width;
      sx_max = (float)(size_constraints.max_size.width) / region.width;
  	sy_min = (float)(size_constraints.min_size.height) / region.height;
      sy_max = (float)(size_constraints.max_size.height) / region.height;
  */
  // Number of global cross entropy iterations ...
  int i;
  for (i = 0; i < params.iterations; i++)
    {

      int count = patches.size();

      int samples_count = 0;

      sample_gaussian2(globalM, globalC, params.min_samples, global_samples, samples_count);

      // clamp the predicted scale
      /*for (int k = 0; k < params.min_samples; k++) {
      	global_samples.at<double>(k, 3) = CLAMP3(global_samples.at<double>(k, 3), sx_min, sx_max) ;
      	global_samples.at<double>(k, 4) = CLAMP3(global_samples.at<double>(k, 4), sy_min, sy_max) ;
      }*/

      for (int k = 0; k < params.min_samples; k++)
        {
          Matrix3f A = simple_affine_transformation(global_samples.at<double>(k, 0), global_samples.at<double>(k, 1),
                       global_samples.at<double>(k, 2), global_samples.at<double>(k, 3), global_samples.at<double>(k, 4));

          PatchCostPair response;
          response.index = k;
          response.cost = 0.0;

          for (int j = 0; j < patches.size(); j++)
            {
              // TODO: optimize this part (tp = A*point + (I-A)*center = A*point + dconst)
              Point2f tp = transform_point(patches.get_relative_position(j, center), A);
              tp.x += center.x;
              tp.y += center.y;
              float cost = exp(- patches.response(image, j, tp) );
              response.cost += cost * patches.get_weight(j);
            }
          sampled_positions[k] = response;
        }

      samples_count = params.min_samples;

      // TODO: not all samples are needed : create a dynamic list of N elite samples data structure
      qsort(sampled_positions, samples_count, sizeof(PatchCostPair), compare_patch_cost_pair);

      while (samples_count < params.max_samples)
        {
          // TODO: verify in CE algorithm (pg. 191)
          if (gamma_low < sampled_positions[params.elite_samples].cost || gamma_high < sampled_positions[0].cost)
            {
              gamma_low = sampled_positions[params.elite_samples].cost;
              gamma_high = sampled_positions[1].cost;
              break;
            }

          sample_gaussian2(globalM, globalC, params.add_samples, global_samples, samples_count);

          // clamp the predicted scale
          /*for (int k = 0; k < params.min_samples; k++) {
          	global_samples.at<double>(k, 3) = CLAMP3(global_samples.at<double>(k, 3), sx_min, sx_max) ;
          	global_samples.at<double>(k, 4) = CLAMP3(global_samples.at<double>(k, 4), sy_min, sy_max) ;
          }*/

          for (int k = samples_count; k < samples_count + params.add_samples; k++)
            {
              Matrix3f A = simple_affine_transformation(global_samples.at<double>(k, 0), global_samples.at<double>(k, 1),
                           global_samples.at<double>(k, 2), global_samples.at<double>(k, 3), global_samples.at<double>(k, 4));
              PatchCostPair response;
              response.index = k;
              response.cost = 0.0;

              for (int j = 0; j < patches.size(); j++)
                {
                  Point2f tp = transform_point(patches.get_relative_position(j, center), A);
                  tp.x += center.x;
                  tp.y += center.y;
                  float cost = exp(-  patches.response(image, j, tp)  ); //exp(- patches.response(image, j, tp)  );
                  response.cost +=  cost * patches.get_weight(j);
                }

              sampled_positions[k] = response;
            }
          samples_count += params.add_samples;

          qsort(sampled_positions, samples_count, sizeof(PatchCostPair), compare_patch_cost_pair);

        }

      for (int j = 0; j < params.elite_samples; j++)
        {
          Mat r = global_elite_samples.row(j);
          global_samples.row(sampled_positions[j].index).copyTo(r);
          global_elite_weights(j, 0) = sampled_positions[j].cost;
        }

      // TODO: fail?
      if (global_elite_weights(0, 0) == 0) global_elite_weights.setTo(1);

      globalC = row_weighted_covariance(global_elite_samples, global_elite_weights);

      globalM = row_weighted_mean(global_elite_samples, global_elite_weights);

      double det = globalC.at<double>(0, 0) * globalC.at<double>(1, 1) - globalC.at<double>(1, 0) * globalC.at<double>(0, 1);

#ifdef BUILD_DEBUG
      if (debug->get_zoom() > 0)
        {
          Mat gray = image.get_gray();
          debug->draw(gray);

          Matrix3f A = simple_affine_transformation(globalM.at<double>(0, 0), globalM.at<double>(0, 1),
                       globalM.at<double>(0, 2), globalM.at<double>(0, 3), globalM.at<double>(0, 4));
          for (int j = 0; j < patches.size(); j++)
            {
              Point2f p = transform_point(status.get_position(j), A, center);
              debug->cross(p, COLOR_RED);
            }

          debug->text(cv::Point(3, 13), string("Global optimization, step: ") + as_string(i), COLOR_RED);

          debug->push(10);

        }
#endif

      if (det < params.terminate || samples_count >= params.max_samples)
        {
          for (int j = 0; j < patches.size(); j++)
            {
              status.converged(j, i);
            }

          break;
        }
    }

  Matrix3f A = simple_affine_transformation(globalM.at<double>(0, 0), globalM.at<double>(0, 1),
               globalM.at<double>(0, 2), globalM.at<double>(0, 3), globalM.at<double>(0, 4));
  for (int j = 0; j < patches.size(); j++)
    {
      // TODO: optimize
      Point2f p = transform_point(status.get_position(j), A, center);
      status.set(j, p, exp(- patches.response(image, j, p)));
    }


  delete [] sampled_positions;

  if (i < params.iterations)
    {
      DEBUGMSG("Global iterations: %d\n", i);
    }
  else
    {
      DEBUGMSG("Warning: global optimization did not converge! \n");
    }
}

void cross_entropy_global_affine2(OptimizationStatus& status, ResponseFunction& function, Matrix& mean, Matrix& covariance, CrossEntropyParameters params, SizeConstraints size_constraints)
{

  if (status.size() == 0)
    return;

  status.reset();

  Matrix globalC = covariance;
  Matrix globalM = mean;

  double gamma_low = 0;
  double gamma_high = 0;

  PatchCostPair* sampled_positions = new PatchCostPair[params.max_samples];

  Mat_<double> global_samples = Mat_<double>(params.max_samples, 5); // this matrix stores global CE parameters
  Mat_<double> global_elite_samples = Mat_<double>(params.elite_samples, 5); // this matrix stores elite global CE parameters
  Mat_<double> global_elite_weights = Mat_<double>(params.elite_samples, 1); // this vector stores elite global CE weighths / responses

  Point2f center(0, 0);
  Rect4f region(INT_MAX, INT_MAX, 0, 0);

  {
    int x2 = 0, y2 = 0;
    for (int i = 0; i < status.size(); i++)
      {
        Point2f p = status.get_position(i);
        region.x = MIN(p.x, region.x);
        region.y = MIN(p.y, region.y);
        x2 = MAX(p.x, x2);
        y2 = MAX(p.y, y2);
        center.x += p.x;
        center.y += p.y;
      }

    region.width = x2 - region.x;
    region.height = y2 - region.y;
    center.x /= status.size();
    center.y /= status.size();
  }

  region.x = -region.width / 2;
  region.y = -region.height / 2;

  float sx_min, sx_max, sy_min, sy_max ;

  sx_min = (float)(size_constraints.min_size.width) / region.width;
  sx_max = (float)(size_constraints.max_size.width) / region.width;
  sy_min = (float)(size_constraints.min_size.height) / region.height;
  sy_max = (float)(size_constraints.max_size.height) / region.height;

  // Number of global cross entropy iterations ...
  int i;
  for (i = 0; i < params.iterations; i++)
    {

      int count = status.size();

      int samples_count = 0;
      sample_gaussian2(globalM, globalC, params.min_samples, global_samples, samples_count);

      // clamp the predicted scale
      for (int k = 0; k < params.min_samples; k++)
        {
          global_samples.at<double>(k, 3) = CLAMP3(global_samples.at<double>(k, 3), sx_min, sx_max) ;
          global_samples.at<double>(k, 4) = CLAMP3(global_samples.at<double>(k, 4), sy_min, sy_max) ;
        }

      for (int k = 0; k < params.min_samples; k++)
        {
          Matrix3f A = simple_affine_transformation(global_samples.at<double>(k, 0), global_samples.at<double>(k, 1),
                       global_samples.at<double>(k, 2), global_samples.at<double>(k, 3), global_samples.at<double>(k, 4));

          PatchCostPair response;
          response.index = k;
          response.cost = 0.0;

          for (int j = 0; j < status.size(); j++)
            {
              // TODO: optimize this part (tp = A*point + (I-A)*center = A*point + dconst)
              Point2f tp = transform_point(status.get_position(j), A, center);
              response.cost += function.response(j, tp);
            }
          response.cost *= function.normalization();
          sampled_positions[k] = response;
        }

      samples_count = params.min_samples;

      // TODO: not all samples are needed : create a dynamic list of N elite samples data structure
      qsort(sampled_positions, samples_count, sizeof(PatchCostPair), compare_patch_cost_pair);

      while (samples_count < params.max_samples)
        {
          // TODO: verify in CE algorithm (pg. 191)
          if (gamma_low < sampled_positions[params.elite_samples].cost || gamma_high < sampled_positions[0].cost)
            {
              gamma_low = sampled_positions[params.elite_samples].cost;
              gamma_high = sampled_positions[1].cost;
              break;
            }

          sample_gaussian2(globalM, globalC, params.add_samples, global_samples, samples_count);

          // clamp the predicted scale
          for (int k = 0; k < params.min_samples; k++)
            {
              global_samples.at<double>(k, 3) = CLAMP3(global_samples.at<double>(k, 3), sx_min, sx_max) ;
              global_samples.at<double>(k, 4) = CLAMP3(global_samples.at<double>(k, 4), sy_min, sy_max) ;
            }

          for (int k = samples_count; k < samples_count + params.add_samples; k++)
            {
              Matrix3f A = simple_affine_transformation(global_samples.at<double>(k, 0), global_samples.at<double>(k, 1),
                           global_samples.at<double>(k, 2), global_samples.at<double>(k, 3), global_samples.at<double>(k, 4));
              PatchCostPair response;
              response.index = k;
              response.cost = 0.0;

              for (int j = 0; j < status.size(); j++)
                {
                  Point2f tp = transform_point(status.get_position(j), A, center);
                  response.cost += function.response(j, tp);
                }

              response.cost *= function.normalization();
              sampled_positions[k] = response;
            }
          samples_count += params.add_samples;

          qsort(sampled_positions, samples_count, sizeof(PatchCostPair), compare_patch_cost_pair);

        }

      for (int j = 0; j < params.elite_samples; j++)
        {
          Mat r = global_elite_samples.row(j);
          global_samples.row(sampled_positions[j].index).copyTo(r);
          global_elite_weights(j, 0) = sampled_positions[j].cost;
        }

      // TODO: fail?
      if (global_elite_weights(0, 0) == 0) global_elite_weights.setTo(1);

      globalC = row_weighted_covariance(global_elite_samples, global_elite_weights);

      globalM = row_weighted_mean(global_elite_samples, global_elite_weights);

      double det = globalC.at<double>(0, 0) * globalC.at<double>(1, 1) - globalC.at<double>(1, 0) * globalC.at<double>(0, 1);

      if (det < params.terminate || samples_count >= params.max_samples)
        {
          for (int j = 0; j < status.size(); j++)
            {
              status.converged(j, i);
            }

          break;
        }
    }

  Matrix3f A = simple_affine_transformation(globalM.at<double>(0, 0), globalM.at<double>(0, 1),
               globalM.at<double>(0, 2), globalM.at<double>(0, 3), globalM.at<double>(0, 4));
  for (int j = 0; j < status.size(); j++)
    {
      // TODO: optimize
      Point2f p = transform_point(status.get_position(j), A, center);
      status.set(j, p, function.response(j, p));
    }



  delete [] sampled_positions;

  if (i < params.iterations)
    {
      DEBUGMSG("Global iterations: %d\n", i);
    }
  else
    {
      DEBUGMSG("Warning: global optimization did not converge! \n");
    }
}


/*
void cross_entropy_global_affine2(ResponseMaps& patches, Matrix& mean, Matrix& covariance, CrossEntropyParameters params, OptimizationStatus* status) {

    for (int i = 0; i < patches.size(); i++) {
        status[i].id = patches.get_id(i);
        status[i].flags = 0;
    }

    Matrix globalC = covariance;
    Matrix globalM = mean;

    Point2f center = patches.mean_position();

    double gamma_low = 0;
    double gamma_high = 0;

    PatchCostPair* sampled_positions = new PatchCostPair[params.max_samples];

    Mat_<double> global_samples = Mat_<double>(params.max_samples, 5); // this matrix stores global CE parameters
    Mat_<double> global_elite_samples = Mat_<double>(params.elite_samples, 5); // this matrix stores elite global CE parameters
    Mat_<double> global_elite_weights = Mat_<double>(params.elite_samples, 1); // this vector stores elite global CE weighths / responses

    // Number of global cross entropy iterations ...
    int i;
    for (i = 0; i < params.iterations; i++) {
        int count = patches.size();

        int samples_count = 0;
        sample_gaussian(globalM, globalC, params.min_samples, global_samples, samples_count);

        for (int k = 0; k < params.min_samples; k++) {
            Matrix3f A = simple_affine_transformation(global_samples.at<double>(k, 0), global_samples.at<double>(k, 1),
                    global_samples.at<double>(k, 2), global_samples.at<double>(k, 3), global_samples.at<double>(k, 4));
            PatchCostPair response; response.index = k; response.cost = 0;
            for (int j = 0; j < patches.size(); j++) {
                Point2f tp = transform_point(patches.get_relative_position(j, center), A);
                tp.x += center.x; tp.y += center.y;
                float cost = exp(- patches.response(j, tp));
                response.cost += isinf(cost) || isnan(cost) ? 0 : cost * patches.get_weight(j);
            }
                if (isinf(response.cost) || isnan(response.cost)) {
                    for (int j = 0; j < patches.size(); j++) {
                        Point2f tp = transform_point(patches.get_relative_position(j, center), A);
                        tp.x += center.x; tp.y += center.y;
                        double c = exp(- patches.response(j, tp));
                        DEBUGMSG("%d - %d %f \n", i, j, c);
                    }
                }
            sampled_positions[k] = response;
        }

        samples_count = params.min_samples;

        qsort(sampled_positions, samples_count, sizeof(PatchCostPair), compare_patch_cost_pair);

        while (samples_count < params.max_samples) {

            if (gamma_low < sampled_positions[params.elite_samples].cost || gamma_high < sampled_positions[0].cost) {
                gamma_low = sampled_positions[params.elite_samples].cost;
                gamma_high = sampled_positions[1].cost;
                break;
            }

            sample_gaussian(globalM, globalC, params.add_samples, global_samples, samples_count);

            for (int k = samples_count; k < samples_count + params.add_samples; k++) {
                Matrix3f A = simple_affine_transformation(global_samples.at<double>(k, 0), global_samples.at<double>(k, 1),
                        global_samples.at<double>(k, 2), global_samples.at<double>(k, 3), global_samples.at<double>(k, 4));
                PatchCostPair response; response.index = k; response.cost = 0;

                for (int j = 0; j < patches.size(); j++) {
                    Point2f tp = transform_point(patches.get_relative_position(j, center), A);
                    tp.x += center.x; tp.y += center.y;
                    float cost = exp(- patches.response(j, tp));
                    response.cost += isinf(cost) || isnan(cost) ? 0 : cost * patches.get_weight(j);
                }

                if (isinf(response.cost) || isnan(response.cost)) {
                    for (int j = 0; j < patches.size(); j++) {
                        Point2f tp = transform_point(patches.get_relative_position(j, center), A);
                        tp.x += center.x; tp.y += center.y;
                        double c = exp(- patches.response(j, tp));
                        DEBUGMSG("%d - %d %f \n", i, j, c);
                    }
                }

                sampled_positions[k] = response;
            }
            samples_count += params.add_samples;

            qsort(sampled_positions, samples_count, sizeof(PatchCostPair), compare_patch_cost_pair);

        }

        for (int j = 0; j < params.elite_samples; j++) {
            Mat r = global_elite_samples.row(j);
            global_samples.row(sampled_positions[j].index).copyTo(r);
            global_elite_weights(j, 0) = sampled_positions[j].cost;
            DEBUGMSG("%d %d - %f \n", i, j, sampled_positions[j].cost);
        }

        if (global_elite_weights(0, 0) == 0) global_elite_weights.setTo(1);

        globalC = row_weighted_covariance(global_elite_samples, global_elite_weights);

        globalM = row_weighted_mean(global_elite_samples, global_elite_weights);

        double det = globalC.at<double>(0, 0) * globalC.at<double>(1, 1) - globalC.at<double>(1, 0) * globalC.at<double>(0, 1);

        if (det < params.terminate || samples_count >= params.max_samples) {
            for (int j = 0; j < patches.size(); j++) {
                status[j].iterations = i;
                status[j].flags |= OPTIMIZATION_CONVERGED;
            }

            break;
        }
    }

    Matrix3f A = simple_affine_transformation(globalM.at<double>(0, 0), globalM.at<double>(0, 1),
            globalM.at<double>(0, 2), globalM.at<double>(0, 3), globalM.at<double>(0, 4));
    for (int j = 0; j < patches.size(); j++) {
        status[j].position = transform_point(patches.get_relative_position(j, center), A);
        status[j].position.x += center.x; status[j].position.y += center.y;
        status[j].value = exp(- patches.response(j, status[j].position));
    }

	delete [] sampled_positions;

    if (i < params.iterations) {
        DEBUGMSG("Global iterations: %d\n", i);
    } else {
        DEBUGMSG("Warning: global optimization did not converge! \n");
    }
}
*/

void cross_entropy_local_refine(Image& image, PatchSet& patches, Constraints& constraints, float covariance, float lambda_geometry, float lambda_visual,
                                CrossEntropyParameters params, OptimizationStatus& status)
{

  status.reset();

  /*for (int i = 0; i < patches.size(); i++) {
      status[i].id = patches.get_id(i);
      status[i].flags ^= OPTIMIZATION_CONVERGED;
  }*/

  int samples = params.min_samples;

  int i;
  vector<Matrix> localC;
  Point2f *localM = new Point2f[patches.size()];
  Point2f *positions = new Point2f[patches.size()];
  bool* done = new bool[patches.size()];
  vector<vector<NeighbourConstraint> > neighborhoods;
  Matrix tempM = (Matrix(1, 2) << 0, 0);
  Matrix tempC = (Matrix(2, 2) << 0, 0, 0, 0);
  Mat_<double> local_samples = Mat_<double>(samples, 2); // this matrix stores local CE parameters
  Mat_<double> local_elite_samples = Mat_<double>(params.elite_samples, 2); // this matrix stores elite local CE parameters
  Mat_<double> local_elite_weights = Mat_<double>(params.elite_samples, 1); // this vector stores elite local CE weighths / responses
  PatchCostPair* local_sampled_positions = new PatchCostPair[samples];
  vector<Point2f> affine_from;
  vector<Point2f> affine_to;
  vector<float> affine_weights;

#ifdef BUILD_DEBUG
  Canvas* debug = get_canvas("optimization");
#endif

  if (patches.size() < 4) return;

  int fixed = 0;

  for (i = 0; i < patches.size(); i++)
    {
      localM[i] = patches.get_position(i);
      assert(!isnan(localM[i].x) && !isnan(localM[i].y));
      if (status.get(i).flags & OPTIMIZATION_FIXED)
        {
          fixed++;
          localC.push_back(Mat::diag( (Mat_<double>(2, 1) << 0, 0)));
          done[i] = true;
        }
      else
        {
          localC.push_back(Mat::diag( (Mat_<double>(2, 1) << covariance, covariance)));
          done[i] = false;
        }
      positions[i] = localM[i];
      neighborhoods.push_back(vector<NeighbourConstraint>());
    }

  DEBUGMSG("Locally fixed patches: %d\n", fixed);

  for (i = 0; i < patches.size(); i++)
    {

      for (int j = 0; j < patches.size(); j++)
        {
          float weight = constraints.constraint(i, j);

          if (weight > 0)
            {
              NeighbourConstraint constraint;
              constraint.index = j;
              constraint.weight = weight;
              neighborhoods[i].push_back(constraint);
            }
        }
    }

  // Number of local cross entropy iterations ...
  for (i = 0; i < params.iterations; i++)
    {

      bool alldone = true;

      for (int p = 0; p < patches.size(); p++)
        {

          if (done[p])
            continue;
          else alldone = false;

          tempM << localM[p].x, localM[p].y;
          tempC = localC[p];

          Point2f neighborhoodSuggest(localM[p].x, localM[p].y);

          if (neighborhoods[p].size() > 2)
            {

              affine_from.clear();
              affine_to.clear();
              affine_weights.clear();

              for (int n = 0; n < neighborhoods[p].size(); n++)
                {
                  affine_from.push_back(positions[neighborhoods[p][n].index]);
                  affine_to.push_back(localM[neighborhoods[p][n].index]);
                  affine_weights.push_back(patches.get_weight(neighborhoods[p][n].index) * neighborhoods[p][n].weight);
                }

              Matrix3f t = compute_affine_transformation(affine_from, affine_to, affine_weights);

              neighborhoodSuggest = transform_point(positions[p], t);

            }

          sample_gaussian2(tempM, localC[p], samples, local_samples, 0);

          /*double* mu_direct = (double *) tempM.data;

          if(isnan(mu_direct[0]) ||isinf(mu_direct[0])||mu_direct[0]==0){
          		float sdf=0 ;
          	}*/

          for (int s = 0; s < samples; s++)
            {
              Point2f tp;
              tp.x = (float)local_samples(s, 0);
              tp.y = (float)local_samples(s, 1);

              float d = distance(neighborhoodSuggest - tp);
              local_sampled_positions[s].cost = exp(- patches.response(image, p, tp) * lambda_visual) * exp(-d * lambda_geometry);
              local_sampled_positions[s].index = s;

              DEBUGGING
              {
                if (isnan(local_sampled_positions[s].cost ) || local_sampled_positions[s].cost == 0)
                  {
                    DEBUGMSG("%d %d %d - %f %f, %f %f - %f \n", i, p, s, tp.x, tp.y, neighborhoodSuggest.x, neighborhoodSuggest.y, d);
                    local_sampled_positions[s].cost = 0;
                  }
              }
            }

          qsort(local_sampled_positions, samples, sizeof(PatchCostPair), compare_patch_cost_pair);
//if (i == 0)printf("P %d\n", p);
          for (int j = 0; j < params.elite_samples; j++)
            {
              Mat r = local_elite_samples.row(j);
              local_samples.row(local_sampled_positions[j].index).copyTo(r);
              local_elite_weights(j, 0) = local_sampled_positions[j].cost;

//if (i == 0)
//printf("%f , %f, %f;\n", local_elite_samples(j, 0), local_elite_samples(j, 1), local_sampled_positions[j].cost);
            }

          if (local_elite_weights(0, 0) == 0) local_elite_weights.setTo(1);

          tempC = row_weighted_covariance(local_elite_samples, local_elite_weights);

          tempM = row_weighted_mean(local_elite_samples, local_elite_weights);

          localM[p].x = tempM(0, 0);
          localM[p].y = tempM(0, 1);
          localC[p] = tempC;



          double det = determinant(tempC);

          status.set(p, localM[p]);

          if (det < params.terminate)
            {
              status.converged(p, i);
              done[p] = true;
            }

#ifdef BUILD_DEBUG

          if (debug->get_zoom() > 0)
            {
              Mat gray = image.get_gray();
              debug->draw(gray);

              Point2f p1 = localM[p];
              for (int n = 0; n < neighborhoods[p].size(); n++)
                {
                  Point2f p2 = localM[neighborhoods[p][n].index];
                  debug->line(p1, p2, COLOR_RED);
                }

              for (int j = 0; j < patches.size(); j++)
                {
                  Point2f p = localM[j];
                  debug->cross(p, COLOR_GREEN);
                }

              debug->text(cv::Point(3, 13), string("Local optimization, step: ") + as_string(i), COLOR_RED);


              debug->push();
            }
#endif

        }

      if (alldone)
        break;

    }

  for (int j = 0; j < patches.size(); j++)
    {
      status.value(j, exp(-patches.response(image, j, status.get_position(j))));
    }


  if (i < params.iterations)
    {
      DEBUGMSG("Local iterations: %d\n", i);
    }
  else
    {
      int converged = 0;
      for (int j = 0; j < patches.size(); j++) converged+= done[j] ? 1 : 0;
      DEBUGMSG("Warning: local optimization did not converge entierly (%d/%d) \n", converged, patches.size());
    }

  delete [] localM;
  delete [] positions;
  delete [] local_sampled_positions;
  delete [] done;
}
/*
void cross_entropy_local_refine2(Image& image, PatchSet& patches, Constraints& constraints, float covariance, float lambda_geometry, float lambda_visual,
    CrossEntropyParameters params, OptimizationStatus* status) {

    for (int i = 0; i < patches.size(); i++) {
        status[i].id = patches.get_id(i);
        status[i].flags ^= OPTIMIZATION_CONVERGED;
    }

    float fix_threshold = 0.8;

    int samples = params.min_samples;

    int i;
    vector<Matrix> localC;
    Point2f *localM = new Point2f[patches.size()];
    Point2f *positions = new Point2f[patches.size()];
    bool* done = new bool[patches.size()];
    int* mode = new int[patches.size()];
    float* visual_best = new float[patches.size()];
    vector<vector<NeighbourConstraint> > neighborhoods;
    Matrix tempM = (Matrix(1, 2) << 0, 0);
    Matrix tempC = (Matrix(2, 2) << 0, 0, 0, 0);
    Mat_<double> local_samples = Mat_<double>(samples, 2); // this matrix stores local CE parameters
    Mat_<double> local_elite_samples = Mat_<double>(params.elite_samples, 2); // this matrix stores elite local CE parameters
    Mat_<double> local_elite_weights = Mat_<double>(params.elite_samples, 1); // this vector stores elite local CE weighths / responses
    PatchCostPair* local_sampled_positions = new PatchCostPair[samples];
    vector<Point2f> affine_from;
    vector<Point2f> affine_to;
    vector<float> affine_weights;

    if (patches.size() < 4) return;


    for (i = 0; i < patches.size(); i++) {
        localM[i] = patches.get_position(i);
        assert(!isnan(localM[i].x) && !isnan(localM[i].y));
        localC.push_back(Mat::diag( (Mat_<double>(2, 1) << covariance, covariance)));
        mode[i] =  (patches.is_active(i) ? ((status[i].value > fix_threshold) ? 1 : 0 ) : -1);
        visual_best[i] = status[i].value;
        done[i] = false; //mode[i] < 0;
        positions[i] = localM[i];
        neighborhoods.push_back(vector<NeighbourConstraint>());
    }

    for (i = 0; i < patches.size(); i++) {

        for (int j = 0; j < patches.size(); j++) {
            float weight = constraints.constraint(i, j);

            if (weight > 0) {
                NeighbourConstraint constraint;
                constraint.index = j;
                constraint.weight = weight;
                neighborhoods[i].push_back(constraint);
            }
        }
    }

    bool nanproblem = false;

    // Number of local cross entropy iterations ...
    for (i = 0; i < params.iterations; i++) {
        bool alldone = true;

    if (nanproblem) return;

        for (int p = 0; p < patches.size(); p++) {

            if (done[p])
                continue;
            else alldone = false;

            //DEBUGMSG("iteration %d, patch %d \n", i, p);

            tempM << localM[p].x, localM[p].y;
            tempC = localC[p];

            Point2f neighborhoodSuggest(localM[p].x, localM[p].y);

            if (neighborhoods[p].size() > 2) {

                affine_from.clear();
                affine_to.clear();
                affine_weights.clear();

                for (int n = 0; n < neighborhoods[p].size(); n++) {
                    affine_from.push_back(positions[neighborhoods[p][n].index]);
                    affine_to.push_back(localM[neighborhoods[p][n].index]);
                    affine_weights.push_back(patches.get_weight(neighborhoods[p][n].index) * neighborhoods[p][n].weight);
                }

                Matrix3f t = compute_affine_transformation(affine_from, affine_to, affine_weights);

                if (!isnan_matrix(t))
                neighborhoodSuggest = transform_point(positions[p], t);

            }

            sample_gaussian(tempM, localC[p], samples, local_samples, 0);

            for (int s = 0; s < samples; s++) {
                Point2f tp;
                tp.x = (float)local_samples(s, 0);
                tp.y = (float)local_samples(s, 1);

                float d = distance(neighborhoodSuggest - tp);

//              local_sampled_positions[s].cost =   (mode[p] > 0 ? 1 :  exp(-patches.response(image, p, tp) * lambda_visual)) * (mode[p] < 0 ? 1 : exp(-d * lambda_geometry));

                local_sampled_positions[s].cost =
                    (mode[p] > 0 ? exp(-patches.response(image, p, tp)) : exp(-patches.response(image, p, tp) * lambda_visual) * exp(-d * lambda_geometry));


                local_sampled_positions[s].index = s;

                if (isnan(local_sampled_positions[s].cost ) || local_sampled_positions[s].cost == 0) {
                    DEBUGMSG("%d %d %d - %f %f, %f %f - %f \n", i, p, s, tp.x, tp.y, neighborhoodSuggest.x, neighborhoodSuggest.y, d);
    nanproblem = true;
                }

            }

            qsort(local_sampled_positions, samples, sizeof(PatchCostPair), compare_patch_cost_pair);

            for (int j = 0; j < params.elite_samples; j++) {
                Mat r = local_elite_samples.row(j);
                local_samples.row(local_sampled_positions[j].index).copyTo(r);
                local_elite_weights(j, 0) = local_sampled_positions[j].cost;
            }

            if (local_elite_weights(0, 0) == 0) local_elite_weights.setTo(1);

            tempC = row_weighted_covariance(local_elite_samples, local_elite_weights);
            tempM = row_weighted_mean(local_elite_samples, local_elite_weights);

            float match = exp(-patches.response(image, p, cv::Point2f(tempM(0, 0), tempM(0, 1))));
            if (match > fix_threshold)
                mode[p] = 1;

            //if (match > visual_best[p]) {
                localM[p].x = tempM(0, 0);
                localM[p].y = tempM(0, 1);
                localC[p] = tempC;
                status[p].position = localM[p];
            //}


            double det = determinant(tempC);

            if (det < params.terminate) {
                status[p].iterations = i;
                status[p].flags |= OPTIMIZATION_CONVERGED;
//                float d = distance(neighborhoodSuggest - status[p].position);
//                status[p].value = exp(- patches.response(image, p, status[p].position));
                done[p] = true;
            }

        }

        if (alldone)
            break;

    }

    for (int j = 0; j < patches.size(); j++) {
        status[j].value = exp(- patches.response(image, j, status[j].position));
    }


    if (i < params.iterations) {
        DEBUGMSG("Local iterations: %d\n", i);
    } else {
        int converged = 0;
        for (int j = 0; j < patches.size(); j++) converged+= done[j] ? 1 : 0;
        DEBUGMSG("Warning: local optimization did not converge entierly (%d/%d) \n", converged, patches.size());
    }

    delete [] localM;
    delete [] positions;
    delete [] done;
    delete [] mode;
	delete [] visual_best;
	delete [] local_sampled_positions;
}*/

}

}