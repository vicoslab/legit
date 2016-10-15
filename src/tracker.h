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

#ifndef LEGIT_TRACKER
#define LEGIT_TRACKER

#include "common/export.h"

namespace legit
{

namespace tracker
{

class __LEGIT_EXPORT Tracker;

class __LEGIT_EXPORT TrackerFactory;

}

}

#include <map>

#include "common/utils/config.h"
#include "common/image/image.h"
#include "common/canvas.h"
#include "observers.h"

using namespace cv;
using namespace std;
using namespace legit::common;

namespace legit
{

namespace tracker
{

class __LEGIT_EXPORT Tracker
{

public:

  virtual void initialize(Image& image, cv::Rect region) = 0;

  virtual void update(Image& image) = 0;

  virtual cv::Rect region() = 0;

  virtual Point2f position() = 0;

  virtual bool is_tracking() = 0;

  virtual void visualize(Canvas& canvas) = 0;

  virtual void add_observer(Ptr<Observer> observer) = 0;

  virtual void remove_observer(Ptr<Observer> observer) = 0;

  virtual vector<TimeStage> get_stages()
  {
    vector<TimeStage> stages;
    return stages;
  }

  virtual string get_name() = 0;

  virtual void set_property(int code, float value);

  virtual float get_property(int code);

  virtual void remove_property(int code);

  virtual bool has_property(int code);

private:

  map<int, float> properties;

};

class __LEGIT_EXPORT ProxyTracker : public Tracker
{

public:

  virtual void initialize(Image& image, cv::Rect region);

  virtual void update(Image& image);

  virtual cv::Rect region();

  virtual Point2f position();

  virtual bool is_tracking();

  virtual void visualize(Canvas& canvas);

  virtual void add_observer(Ptr<Observer> observer);

  virtual void remove_observer(Ptr<Observer> observer);

  virtual vector<TimeStage> get_stages();

  virtual string get_name();

  virtual void set_property(int code, float value);

  virtual float get_property(int code);

  virtual void remove_property(int code);

  virtual bool has_property(int code);

protected:

  Ptr<Tracker> tracker;

};

typedef Tracker* (*CreateTrackerFunc)(Config& config, string id);
typedef std::map<std::string, CreateTrackerFunc> TrackerRegistry;

inline TrackerRegistry& getTrackerRegistry()
{
  static TrackerRegistry reg;
  return reg;
}

template<class T>
Tracker* createTracker(Config& config, string id)
{
  return new T(config, id);
}

template<class T>
struct RegistryEntry
{
public:
  static RegistryEntry<T>& Instance(const std::string& name)
  {
    // Because I use a singleton here, even though `REGISTER_TRACKER`
    // is expanded in multiple translation units, the constructor
    // will only be executed once. Only this cheap `Instance` function
    // (which most likely gets inlined) is executed multiple times.

    static RegistryEntry<T> inst(name);
    return inst;
  }

private:
  RegistryEntry(const std::string& name)
  {
    TrackerRegistry& reg = getTrackerRegistry();
    CreateTrackerFunc func = createTracker<T>;

    DEBUGMSG("Registering tracker for name %s\n", name.c_str());

    std::pair<TrackerRegistry::iterator, bool> ret =
      reg.insert(TrackerRegistry::value_type(name, func));

    if (ret.second == false)
      {
        // This means there already is a tracker registered to this name.
        throw LegitException("Tracker already registered for this name");
      }
  }

  RegistryEntry(const RegistryEntry<T>&) = delete; // C++11 feature
  RegistryEntry& operator=(const RegistryEntry<T>&) = delete;
};

__LEGIT_EXPORT Tracker* create_tracker(string type, Config& config, string id);

__LEGIT_EXPORT vector<string> list_builtin_configs();

__LEGIT_EXPORT vector<string> list_registered_trackers();

__LEGIT_EXPORT void read_builtin_config(string name, Config& config);

}

}

#define REGISTER_TRACKER(TYPE, NAME)                                          \
    namespace legit {                                                         \
    namespace tracker {                                                       \
    namespace                                                                 \
    {                                                                         \
        template<class T>                                                     \
        class TrackerRegistration;                                            \
                                                                              \
        template<>                                                            \
        class TrackerRegistration<TYPE>                                       \
        {                                                                     \
            static const ::legit::tracker::RegistryEntry<TYPE>& reg;          \
        };                                                                    \
                                                                              \
        const ::legit::tracker::RegistryEntry<TYPE>&                          \
            TrackerRegistration<TYPE>::reg =                                  \
                ::legit::tracker::RegistryEntry<TYPE>::Instance(NAME);        \
    }}}

#define MAKE_TITLE(I, T) ("[" + inst + "] " + T)

#endif