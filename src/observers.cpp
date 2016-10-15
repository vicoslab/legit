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

#include "observers.h"

namespace legit
{

namespace tracker
{

PerformanceObserver::PerformanceObserver(vector<TimeStage> stages) : stages(stages.begin(), stages.end())
{

  previous_stage = -1;

}

PerformanceObserver::~PerformanceObserver()
{

}

void PerformanceObserver::notify(Tracker* tracker, int channel, void* data, int flags)
{
  if (channel != OBSERVER_CHANNEL_MAIN)
    return;

  int stage = * ((int *)data);

  long current_time = clock();

  switch (stage)
    {
    case STAGE_BEGIN:
    {
      vector<long> frame;
      for (size_t i = 0; i < stages.size(); i++)
        frame.push_back(0);
      frames.push_back(frame);
      time = clock();
      return;
    }
    default:

      if (previous_stage > -1)
        frames[frames.size()-1][previous_stage] = (current_time - time) + 1;

      for (size_t i = 0; i < stages.size(); i++)
        {
          if (stage == stages[i].id)
            {
              previous_stage = i;
              break;
            }

        }
      break;
    }

  time = clock();

}

#define CLOCKS_PER_MILISEC CLOCKS_PER_SEC / 1000

void PerformanceObserver::print()
{
  if (stages.size() == 0 || frames.size() == 0)
    return;

  long total = 0;

  vector<long> average;
  for (size_t i = 0; i < stages.size(); i++)
    {
      average.push_back(0);
    }

  for (size_t j = 0; j < frames.size(); j++)
    {

      vector<long> frame = frames[j];

      for (size_t i = 0; i < stages.size(); i++)
        {
          average[i] += frame[i];
          total += frame[i];
        }
    }

  printf("Performance summary for %d frames: \n\n", (int)frames.size());

  for (size_t i = 0; i < stages.size(); i++)
    {
      printf("\t* %s\t%ldms \n", stages[i].name.c_str(), ((average[i]) / (CLOCKS_PER_MILISEC * frames.size())));
    }

  printf("\t-----------------\n\tTotal time: %ldms \n\n", (((total)) / (CLOCKS_PER_MILISEC * frames.size())));
}

}

}
