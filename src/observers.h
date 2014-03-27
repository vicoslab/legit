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

#ifndef LEGIT_OBSERVERS
#define LEGIT_OBSERVERS

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#ifdef PLATFORM_WINDOWS
#include <time.h>
#endif

#define OBSERVER_CHANNEL_MAIN 1
#define OBSERVER_CHANNEL_STRUCTURE 2
#define OBSERVER_CHANNEL_OPTIMIZATION 3
#define OBSERVER_CHANNEL_REWEIGHT 4
#define OBSERVER_CHANNEL_PATCH_ADD 5

#define OBSERVER_CHANNEL_INITIALIZE 100

#define OBSERVER_FLAG_INITIALIZE 1
#define OBSERVER_FLAG_FRAME_START 2
#define OBSERVER_FLAG_FRAME_END 4

#define STAGE_BEGIN 0
#define STAGE_OPTIMIZATION_GLOBAL 2
#define STAGE_OPTIMIZATION_LOCAL 3
#define STAGE_UPDATE_WEIGHTS 4
#define STAGE_REMOVE_PATCHES 5
#define STAGE_UPDATE_MODALITIES 6
#define STAGE_ADD_PATCHES 7
#define STAGE_END -1

using namespace std;

namespace legit
{

namespace tracker
{

class Observer;

typedef struct
{
  int id;
  vector<float> weights;
} PatchReweight;

}

class TimeStage
{
public:
  TimeStage(string name, int id) : name(name), id(id) {};
  ~TimeStage() {};

  int id;
  string name;

};

}

#include "tracker.h"

namespace legit
{

namespace tracker
{

class Observer
{
public:
  Observer() {};
  virtual ~Observer() {};
  virtual void notify(Tracker* tracker, int channel, void* data, int flags) = 0;
};

class PerformanceObserver : public Observer
{
public:
  PerformanceObserver(vector<TimeStage> stages);
  virtual ~PerformanceObserver();

  virtual void notify(Tracker* tracker, int channel, void* data, int flags);
  void print();

private:

  int previous_stage;

  long time;

  vector<TimeStage> stages;

  vector<vector<long> > frames;

};

}

}

#endif