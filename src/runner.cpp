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

#include "tracker.h"
#include "common/utils/defs.h"
#include "common/utils/string.h"
#include "common/platform/filesystem.h"
#include "common/gui/gui.h"
#include "common/gui/window.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include "common/platform/windows/getopt_win.h"
inline void sleep(long time)
{
  Sleep(time);
}
#else
#include <unistd.h>
#endif

#ifdef BUILD_TRAX
#include <trax.h>
#endif

#define CMD_OPTIONS "hc:C:dgsiI:S:tD:o:"

using namespace legit;
using namespace legit::tracker;
using namespace legit::common;
#ifdef BUILD_INTROSPECTION
using namespace legit::introspection;
#endif

#ifdef BUILD_TRAX
class TraxObserver : public Observer
{
public:

  trax_properties* properties;

  TraxObserver()
  {

    properties = trax_properties_create();
  };


  virtual ~TraxObserver()
  {
    trax_properties_release(&properties);
  };


  virtual void notify(Tracker* tracker, int channel, void* data, int flags)
  {

    switch (channel)
      {
      case OBSERVER_CHANNEL_MAIN:
      {

        int stage = * ((int *)data);

        if (stage == STAGE_BEGIN)
          trax_properties_clear(properties);

        break;
      }
      /*
      case OBSERVER_CHANNEL_STRUCTURE: {
          PatchSet* patches = (PatchSet *) data;
          trax_properties_set_int(properties, "legit.pch.size", patches->size());
          char tmp_key[32];
          char tmp_value[512];

          for (int i = 0; i < patches->size(); i++) {
              Point2f p = patches->get_position(i);
              sprintf(tmp_key, "legit.pch.%d", i);
              sprintf(tmp_value, "%d,%f,%f,%f", patches->get_id(i), p.x, p.y, patches->get_weight(i));
              trax_properties_set(properties, tmp_key, tmp_value);
          }

          break;
          }
      case OBSERVER_CHANNEL_OPTIMIZATION: {

          break;
          }
          */
      }

  };

};
#endif

void print_help()
{

  cout << "LEGIT by ViCoS Lab" << "\n";
  cout << "Built on " << __DATE__ << " at " << __TIME__ << "\n\n";

  cout << "Usage: tracker [-h] [-d] [-g] [-s] [-i] [-t] \n";
  cout << "\t [-C config_file] [-c config] [-I initialize_file]\n";
  cout << "\t [-S seed] [-D dump_file] [-o output_file] <source>";

  cout << "\n\nProgram arguments: \n";
  cout << "\t-h\tPrint this help and exit\n";
#ifdef BUILD_DEBUG
  cout << "\t-d\tEnable debug mode\n";
#endif
  cout << "\t-g\tGUI mode\n";
  cout << "\t-s\tSilent mode\n";
  cout << "\t-i\tInteractive mode\n";
#ifdef BUILD_TRAX
  cout << "\t-t\tTrax interface mode (overrides most of the other flags)\n";
#endif
  cout << "\t-c\tSpecify custom configuration as semicolon-delimited string\n";
  cout << "\t-C\tSpecify configuration file (can also be a name of a built-in resource)\n";
  cout << "\t-I\tSpecify initialization bounding-box file\n";
#ifdef BUILD_INTROSPECTION
  cout << "\t-D\tDump introspection data to a file\n";
#endif
  cout << "\t-S\tSpecify seed for random generator\n";
  cout << "\t-o\tSpecify an output bounding-boxes file\n";
  cout << "\n";

  cout << "\nSource can be in one of the following formats:\n";
  cout << "\tvideo_file[?from:to]\n\t\tA video file that can be decoded using OpenCV\n";
  cout << "\timage_dir[?from:to]\n\t\tA path to a directory that will be scanned for images\n";
  cout << "\timage_mask[?from:to]\n\t\tA path mask using printf notation that will be given a single integer\n";
  cout << "\t:[camera_id]\n\t\tA camera that can be accessed using OpenCV\n";

  vector<string> tracker_list = list_registered_trackers();

  if (tracker_list.size() > 0)
    {
      cout << "\nList of available trackers:\n";

      for (size_t i = 0; i < tracker_list.size(); i++)
        {

          cout << "\t" << tracker_list[i] << "\n";
        }
    }

  vector<string> config_list = list_builtin_configs();

  if (config_list.size() > 0)
    {
      cout << "\nList of available built-in configurations:\n";

      for (size_t i = 0; i < config_list.size(); i++)
        {

          cout << "\t" << config_list[i] << "\n";
        }
    }

  cout << "\n";
}

Rect read_rectangle(const char* filename)
{

  std::ifstream in(filename);
  string item;
  Rect rect(0, 0, 0, 0);

  if (getline (in, item, ','))
    {
      rect.x = atoi(item.c_str());
    }
  if (getline (in, item, ','))
    {
      rect.y = atoi(item.c_str());
    }
  if (getline (in, item, ','))
    {
      rect.width = atoi(item.c_str());
    }
  if (getline (in, item, ','))
    {
      rect.height = atoi(item.c_str());
    }

  return rect;
}

void write_rectangle(const char* filename, Rect rectangle)
{

  std::ofstream out( filename , std::ifstream::out);

  if( !out ) return;

  out << rectangle.x << "," << rectangle.y << "," << rectangle.width << "," << rectangle.height;

}

int pause()
{

  while(true)
    {
      char key = waitKey(10);
      if(key >= 0)
        {
          DEBUGMSG("Key pressed: %d \n", key);
          return key;
        }
    }

  return -1;
}

void trax_properties_to_config(const char *key, const char *value, const void *obj)
{
  Config* config = (Config*) obj;

  config->add<string>(string(key), string(value));

}

bool traxmode = false;
bool silent = false;
bool gui = false;
bool interactive;
char* initializeFile = NULL;
char* configFile = NULL;
char* configString = NULL;
char* introspectionFile = NULL;
char* outputFile = NULL;
int seed;
cv::Rect start(0, 0, 100, 100);

Sequence* sequence;

bool initialized = false;
bool initialize = false;
bool run = true;

long limitFrameTime = 50;
bool throttle = false;

Ptr<Observer> performance_observer;
Ptr<Observer> trax_observer;
Ptr<Observer> introspection_observer;

ImageWindow* tracking_window = NULL;

Config config;

#define WINDOW_NAME "Legit main window"

int main( int argc, char** argv)
{

// Initialization

  traxmode = false;
  silent = false;
  interactive = false;
  opterr = 0;
  initializeFile = NULL;
  configFile = NULL;
  configString = NULL;
  outputFile = NULL;

#ifdef BUILD_INTROSPECTION
  introspectionFile = NULL;
#endif
  int c = 0;
  seed = time(NULL);

  initialized = false;
  initialize = false;
  run = true;

  while ((c = getopt(argc, argv, CMD_OPTIONS)) != -1)
    switch (c)
      {
      case 'h':
        print_help();
        exit(0);
      case 's':
        silent = true;
        break;
      case 'g':
        enable_gui();
        break;
      case 'i':
        interactive = true;
        break;
#ifdef BUILD_DEBUG
      case 'd':
        __debug_enable();
        break;
#endif
      case 'C':
        configFile = optarg;
        break;
      case 'c':
        configString = optarg;
        break;
      case 'I':
        initializeFile = optarg;
        break;
      case 'o':
        outputFile = optarg;
        break;
#ifdef BUILD_INTROSPECTION
      case 'D':
        introspectionFile = optarg;
        break;
#endif
      case 'S':
        seed = atoi(optarg);
        break;
#ifdef BUILD_TRAX
      case 't':
        traxmode = true;
        break;
#endif
      default:
        print_help();
        fprintf(stderr, "Unknown switch '-%c'\n", optopt);
        exit(-1);
      }

  RANDOM_SEED(seed);

  DEBUGMSG("Random seed: %d \n", seed);

  sequence = NULL;

  if (!traxmode)
    {

      if (optind < argc)
        {

          sequence = open_sequence(argv[optind]);

          if (!sequence)
            {
              fprintf(stderr, "Illegal source\n");
              exit(-1);
            }
          VideoFileSequence* videostream = dynamic_cast<VideoFileSequence*>(sequence);
          ImageDirectorySequence* dirstream = dynamic_cast<ImageDirectorySequence*>(sequence);
          FileListSequence* liststream = dynamic_cast<FileListSequence*>(sequence);
          if (videostream != NULL)
            {

              const char* filename = videostream->get_file();
              initialize = true;

              if (!initializeFile && filename)
                {
                  initializeFile = new char[strlen(filename) + 5];
                  strcpy(initializeFile, filename);
                  strcpy(initializeFile+strlen(filename), ".txt");
                }

            }
          else if (dirstream != NULL)
            {

              const char* filename = dirstream->get_directory();
              initialize = true;

              if (!initializeFile && filename)
                {
                  initializeFile = new char[strlen(filename) + 5];
                  strcpy(initializeFile, filename);
                  strcpy(initializeFile + strlen(filename) +
                         (matches_suffix(filename, FILE_DELIMITER_STR) ? -1 : 0),
                         ".txt");
                }

            }
          else if (liststream != NULL && initializeFile)
            {

              initialize = true;

            }
          else if (dynamic_cast<CameraSequence*>(sequence) != NULL)
            {

              interactive = false;

            }

        }
      else
        {
          fprintf(stderr, "No source given\n");
          exit(-1);
        }
    }

// Tracking

  tracking_window = is_gui_enabled() ? ImageWindow::create(WINDOW_NAME, Size(10, 10), false) : NULL;

  if (configFile)
    {
      if (file_type(configFile) == FILETYPE_FILE)
        {
          DEBUGMSG("Reading config from '%s' \n", configFile);
          config.load_config(string(configFile));
        }
      else
        {
          DEBUGMSG("Reading built-in configuration '%s' \n", configFile);
          read_builtin_config(configFile, config);
        }
    }

  if (configString)
    {
      for (size_t i = 0; i < strlen(configString); i++)
        {
          if (configString[i] == ';') configString[i] = '\n';
        }
      istringstream moreConfig(configString);
      moreConfig >> config;
    }

  DEBUGGING
  {

    vector<string> configKeys = config.keys();
    for (size_t i = 0; i < configKeys.size(); i++)
      {
        string value = config.read<string>(configKeys[i]);
        DEBUGMSG("Config dump: %s = %s\n", configKeys[i].c_str(), value.c_str());
      }

  }

  initialize_canvases(config);

  if (config.keyExists("window.main.output"))
    {
      tracking_window->set_output(config.read<string>("window.main.output"), config.read<int>("window.main.output.slowdown", 1));

    }

#ifdef BUILD_TRAX
  trax_properties* trax_out_properties = NULL;
  if (traxmode)
    {
#ifdef TRAX_DEBUG
      trax_setup("trax.log", TRAX_LOG_INPUT |  TRAX_LOG_OUTPUT | TRAX_LOG_DEBUG);
#else
      trax_setup(NULL, 0);
#endif
      trax_out_properties = trax_properties_create();
    }
#endif

  Ptr<Tracker> tracker;
  Image frame;

  int frameNumber = 1;

  ImageWindow::grid(2, 3, 400, 400);


  std::ofstream rectangle_output;
  if (outputFile)
    rectangle_output.open( outputFile , std::ifstream::out);

  for(; run;)
    {

      long start_time = clock();

      char key = -1;

      /////////////////////         GET IMAGE             ////////////////////

      if (!traxmode)
        {
          frame.capture(sequence);
        }
#ifdef BUILD_TRAX
      else
        {

          trax_imagepath path;
          trax_rectangle rect;
          trax_properties* prop = trax_properties_create();

          int tr = trax_wait(path, prop, &rect);

          if (tr == TRAX_INIT)
            {

              start.x = rect.x;
              start.y = rect.y;
              start.width = rect.width;
              start.height = rect.height;

              trax_properties_enumerate(prop, trax_properties_to_config, &config);

              initialize = true;

              initialized = false;

            }
          else if (tr == TRAX_FRAME)
            {

              if (!initialized)
                {
                  trax_properties_release(&prop);
                  break;
                }
            }
          else
            {
              trax_properties_release(&prop);
              break;
            }

          trax_properties_release(&prop);

          frame.load(string(path));

          trax_properties_clear(trax_out_properties);
        }
#endif

      if (frame.empty())
        {
          DEBUGMSG("No new frame available, quitting.\n");
          break;
        }

      if (frameNumber == 1 && !traxmode)
        {
          if (is_gui_enabled()) tracking_window->resize(Size(frame.width(), frame.height()));
          int size = MAX(frame.width() / 8,  frame.height() / 8);
          start.width = size;
          start.height = size;
          start.x = (frame.width() - start.width) / 2;
          start.y = (frame.height() - start.height) / 2;
        }

      /////////////////////         PROCESSING            ////////////////////

      if (!initialized && initialize)
        {
          DEBUGMSG("Initializing using rectangle from '%s'\n", initializeFile);
          if (initializeFile)
            {
              DEBUGMSG("Initializing using rectangle from '%s'\n", initializeFile);
              start = read_rectangle(initializeFile);
              if (is_gui_enabled() && (start.width < 1 || start .height < 1))
                {
                  Mat rgb = frame.get_rgb();
                  tracking_window->draw(rgb);
                  tracking_window->push();
                  if (tracking_window->queryrect(start))
                    {
                      write_rectangle(initializeFile, start);
                    }
                }
            }

          if (tracker.empty())
            {

              if (!config.keyExists("tracker"))
                throw LegitException("Unknown tracker type");

              tracker = create_tracker(config.read<string>("tracker"), config, "default");

              if (!silent)
                {
                  performance_observer = new PerformanceObserver(tracker->get_stages());
                  tracker->add_observer(performance_observer);
                }
#ifdef BUILD_TRAX
              if (traxmode)
                {
                  trax_observer = new TraxObserver();
                  tracker->add_observer(trax_observer);
                  trax_properties_set_int(trax_out_properties, "seed", seed);
                }
#endif

#ifdef BUILD_INTROSPECTION
              if (introspectionFile)
                {
                  introspection_observer = new IntrospectionObserver(frame.width(), frame.height(), seed);
                  ((IntrospectionObserver *)&(*introspection_observer))->configuration(config);
                  tracker->add_observer(introspection_observer);
                }
#endif
            }

          tracker->initialize(frame, start);
          initialized = true;

        }
      else if (!tracker.empty())
        {
          frameNumber++;
          if (!traxmode && sequence->is_finite() && sequence->position() >= sequence->size())
            {
              DEBUGMSG("End frame reached, quitting.\n");
              run = false;
            }

          long timer = clock();
          tracker->update(frame);

          if (!silent && !traxmode) printf("Frame %d - elapsed time: %d ms\n", frameNumber, (int)(((clock() - timer) * 1000) / CLOCKS_PER_SEC));
        }

      /////////////////////       OUTPUT RESULTS          ////////////////////

#ifdef BUILD_TRAX
      if (traxmode)
        {
          trax_rectangle region;

          if (tracker->is_tracking())
            {

              Rect rect = tracker->region();
              region.x = rect.x;
              region.y = rect.y;
              region.width = rect.width;
              region.height = rect.height;

            }
          else
            {

              region.x = 0;
              region.y = 0;
              region.width = 0;
              region.height = 0;

            }

          trax_report_position(region, trax_out_properties);// properties);

        }
#endif

      if (is_gui_enabled() && tracking_window)
        {
          Mat rgb = frame.get_rgb();
          tracking_window->draw(rgb);

          if (initialized)
            {

              tracker->visualize(*tracking_window);

              cv::Rect region = tracker->region();

              tracking_window->rectangle(region, COLOR_YELLOW, 2);

            }
          else
            {

              tracking_window->rectangle(start.tl(), start.br(), Scalar(255, 255, 255), 1);
            }

          tracking_window->push();
        }

      if (rectangle_output.is_open())
        {
          cv::Rect rectangle = initialized ? tracker->region() : start;
          rectangle_output << rectangle.x << "," << rectangle.y << "," << rectangle.width << "," << rectangle.height << std::endl;
        }

      /////////////////////        HANDLE INPUT           ////////////////////

      if (!traxmode)
        {
          if (is_gui_enabled())
            {

              if (interactive)
                {

                  switch (pause())
                    {
                    case 27:
                      run = false;
                      break;
                    case 'p':
                      interactive = !interactive;
                      break;
                    }

                }
              else
                {

                  long wait = throttle ? MAX(1, (limitFrameTime - (clock() - start_time))) : 1;
                  key = waitKey(wait);
                  switch (key)
                    {
                    case 27:
                      run = false;
                      break;
                    case ' ':
                      initialize = true;
                      break;
                    }

                }
            }
          else
            {
              if (throttle)
                {
                  long wait = MAX(1, (limitFrameTime - (clock() - start_time)));
                  sleep(wait);
                }
            }

          if (!tracker.empty() && !tracker->is_tracking())
            {
              DEBUGMSG("Target lost\n");
              run = false;
            }

        }
#ifdef BUILD_TRAX
      else
        {
          if (is_gui_enabled()) waitKey(1);
        }
#endif

    }

  if (!performance_observer.empty())
    {
      ((PerformanceObserver *)&(*performance_observer))->print();
    }

// Cleanup

#ifdef BUILD_INTROSPECTION
  if (!introspection_observer.empty())
    {
      DEBUGMSG("Storing introspection data to %s\n", introspectionFile);
      ((IntrospectionObserver *)&(*introspection_observer))->store(introspectionFile, true);
    }
#endif

#ifdef BUILD_TRAX
  trax_properties_release(&trax_out_properties);
  if (traxmode)
    trax_cleanup();
#endif

  if (rectangle_output.is_open())
    {
      rectangle_output.close();
    }

  if (sequence)
    delete sequence;

}
