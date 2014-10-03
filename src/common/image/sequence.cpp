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

#include <iostream>
#include <fstream>

#include "common/utils/debug.h"
#include "common/utils/string.h"
#include "common/image/sequence.h"
#include "common/platform/filesystem.h"
#include <opencv2/imgproc/imgproc.hpp>

namespace legit
{

namespace common
{


////////////// Video file sequence

VideoFileSequence::VideoFileSequence(const char* filenamein, int start, int end) : file_start(1), sequence_file(NULL)
{

  cap.open(filenamein);

  file_end = cap.get(CV_CAP_PROP_FRAME_COUNT) - 1;
  current_frame = 0;

  if(!cap.isOpened())
    throw LegitException("Unable to read file as video");

  if (start > -1) file_start = MIN(file_end-2, MAX(1, start));
  if (end > -1) file_end = MIN(file_end, MAX(file_start+1, end));

  sequence_file = new char[strlen(filenamein) + 1];
  strcpy(sequence_file, filenamein);

  skip(0);
}


VideoFileSequence::~VideoFileSequence()
{

  if (sequence_file)
    delete [] sequence_file;

}

bool VideoFileSequence::read_frame(Mat& img)
{

  if (current_frame >= size())
    return false;

  cap >> img;

  current_frame = cap.get(CV_CAP_PROP_POS_FRAMES) - file_start + 1;

  if (current_frame >= size()) img.release();

  return !img.empty();

}

int VideoFileSequence::position()
{

  return current_frame;

}

int VideoFileSequence::size()
{

  return (file_end - file_start) + 1;

}

int VideoFileSequence::skip(int position)
{

  if (current_frame == position)
    return current_frame;

  position = MIN(size(), MAX(0, position));

  current_frame = position;
  cap.set(CV_CAP_PROP_POS_FRAMES, position + file_start - 1);

  return current_frame;

}

int VideoFileSequence::width()
{
  return cap.get(CV_CAP_PROP_FRAME_WIDTH);

}

int VideoFileSequence::height()
{
  return cap.get(CV_CAP_PROP_FRAME_HEIGHT);

}

const char* VideoFileSequence::get_file()
{

  return sequence_file;

}

////////// Camera sequence

CameraSequence::CameraSequence(int camera)
{

  //TODO: Make capture size configurable
  cap.open(camera);
  cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

  current_frame = 0;

}

CameraSequence::~CameraSequence()
{


}

bool CameraSequence::read_frame(Mat& img)
{

  cap >> img;

  if (!img.empty())
    {
      current_frame++;
      return true;
    }
  else return false;

}

int CameraSequence::position()
{

  return current_frame;

}

int CameraSequence::size()
{

  return -1;

}

int CameraSequence::skip(int position)
{

  return current_frame;

}

int CameraSequence::width()
{
  return cap.get(CV_CAP_PROP_FRAME_WIDTH);

}

int CameraSequence::height()
{
  return cap.get(CV_CAP_PROP_FRAME_HEIGHT);

}

ImageFileListSequence::ImageFileListSequence() : file_start(1)
{

}

ImageFileListSequence::~ImageFileListSequence()
{

}

bool ImageFileListSequence::load_list()
{

  sequence_width = 0;
  sequence_height = 0;

  current_frame = 0;

  if (size() > 0)
    {
      Mat frm = imread(files[0].c_str());
      sequence_width = frm.cols;
      sequence_height = frm.rows;
      frm.release();
    }

}

bool ImageFileListSequence::read_frame(Mat& img)
{

  if (current_frame < size())
    {

      DEBUGMSG("Reading image file '%s'\n", files[current_frame + file_start - 1].c_str());
      Mat tmp = imread(files[current_frame + file_start - 1].c_str());

      if (tmp.empty())
        {
          img.release();
          return false;
        }

      if (sequence_width == img.cols && sequence_height == img.rows)
        {
          tmp.assignTo(img);
        }
      else resize(tmp, img, cv::Size(sequence_width, sequence_height));

    }
  else img.release();

  if (!img.empty())
    current_frame++;

  return !img.empty();

}

int ImageFileListSequence::position()
{

  return current_frame;

}

int ImageFileListSequence::size()
{

  return (file_end - file_start) + 1;

}

int ImageFileListSequence::skip(int position)
{

  if (current_frame == position)
    return current_frame;

  current_frame = MIN(size(), MAX(1, position + file_start - 1));

  return current_frame;
}

int ImageFileListSequence::width()
{
  return sequence_width;

}

int ImageFileListSequence::height()
{
  return sequence_height;

}

void ImageFileListSequence::list_files()
{

  for (int i = file_start; i < file_end; i++)
    {
      cout << files[i] << endl;
    }

}

FileListSequence::FileListSequence(const char* listing) : sequence_listing(listing), ImageFileListSequence()
{

  load_list();

  file_end = files.size();

}

FileListSequence::~FileListSequence()
{


}

const char* FileListSequence::get_listing()
{

  return sequence_listing.c_str();

}

bool FileListSequence::load_list()
{

  read_listing(sequence_listing);

  ImageFileListSequence::load_list();

}

bool FileListSequence::read_listing(const string listing)
{

  std::string line;

  std::ifstream listing_stream (listing.c_str());

  if (listing_stream.is_open())
    {

      while (listing_stream.good())
        {

          getline(listing_stream, line);
//DEBUGMSG("%s %d \n", line.c_str(),file_type(line.c_str()));
          if (file_type(line.c_str()) != FILETYPE_FILE)
            continue;

          files.push_back(string(line));

        }

      listing_stream.close();
    }
  else throw LegitException("Unable to read file");

}

ImageDirectorySequence::ImageDirectorySequence(const char* directory, int start, int end) : sequence_source(directory), ImageFileListSequence()
{

  scan_directory(string(directory));

  file_end = files.size();

  if (start > -1) file_start = MIN(file_end-2, MAX(1, start));
  if (end > -1) file_end = MIN(file_end, MAX(file_start+1, end));

  load_list();


}

ImageDirectorySequence::~ImageDirectorySequence()
{


}

const char* ImageDirectorySequence::get_directory()
{

  return sequence_directory.c_str();

}

bool ImageDirectorySequence::load_list()
{

  scan_directory(sequence_source);

  ImageFileListSequence::load_list();

}

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <iostream>
#endif

bool ImageDirectorySequence::scan_directory(const string& dirstr)
{

  const char* dir = dirstr.c_str();

  files.clear();

  int type = file_type(dir);

  sequence_directory = sequence_source;

  DEBUGMSG("Scanning %s\n", dirstr.c_str());

  if (type == FILETYPE_DIRECTORY)
    {

#ifdef PLATFORM_WINDOWS
      // TODO
      char tmpDirSpec[MAX_PATH+1];
      sprintf (tmpDirSpec, "%s/*", dir);
      WIN32_FIND_DATA f;
      HANDLE h = FindFirstFile(tmpDirSpec , &f);
      if(h != INVALID_HANDLE_VALUE)
        {
          FindNextFile(h, &f);	//read ..
          FindNextFile(h, &f);	//read .
          do
            {
              files.push_back(f.cFileName);
            }
          while(FindNextFile(h, &f));
        }
      FindClose(h);

      sort (files.begin(), files.end());
#else

      DIR *dp;
      struct dirent *dirp;
      if((dp  = opendir(dir)) == NULL)
        {
          DEBUGMSG("Error (%d) opening %s \n", errno, dir);
          return false;
        }

      string dirstr(dir);

      while ((dirp = readdir(dp)) != NULL)
        {
          if (matches_suffix(dirp->d_name, ".jpg") || matches_suffix(dirp->d_name, ".png"))
            files.push_back(path_join(dirstr, string(dirp->d_name)));
        }
      closedir(dp);

      sort (files.begin(), files.end());

#endif

      //sequence_directory = new char[strlen(dir) + 1];
      //strcpy(sequence_directory.c, dir);

    }
  else if (type == FILETYPE_NONE)
    {

      char* buffer = new char[strlen(dir) + 100];

      int n = 1;

      while (true)
        {

          sprintf(buffer, dir, n);
          FILE *fp = fopen(buffer,"r");
          if( fp )
            {
              files.push_back(string(buffer));
              fclose(fp);
            }
          else
            {
              break;
            }

          n = n + 1;
        }

      delete [] buffer;

      int dirend = (int)(strrchr(dir, FILE_DELIMITER) - dir);

      if (dirend > -1)
        {
          sequence_directory = sequence_source.substr(0, dirend); // new char[dirend + 1];
          //strncpy(sequence_directory.c_str(), dir, dirend);
        }

    }

  DEBUGMSG("Found %d images\n", (int)files.size());

  return true;

}


Sequence* open_sequence(const char* name)
{

  Sequence* sequence = NULL;

  if (!matches_prefix(name, ":"))
    {

      char* filenametmp = new char[strlen(name)+1];
      strcpy(filenametmp, name);

      int file_start = 1;
      int file_end = -1;

      const char* filename = strtok(filenametmp, "?");
      const char* start = strtok(NULL, ":");
      const char* end = strtok(NULL, ":");
      if (start != NULL) file_start = MAX(1, atoi(start));
      if (end != NULL) file_end = MAX(file_start+1, atoi(end));

      if (file_type(filename) == FILETYPE_FILE)
        {

          if (matches_suffix(filename, ".avi") || matches_suffix(filename, ".mov") ||
              matches_suffix(filename, ".mp4") || matches_suffix(filename, ".mpg"))
            {
              sequence = new VideoFileSequence(filename, file_start, file_end);
              DEBUGMSG("Input video file %s\n", filename);
            }
          else
            {
              sequence = new FileListSequence(filename);
              DEBUGMSG("Input image list file %s\n", filename);
            }

        }
      else
        {

          sequence = new ImageDirectorySequence(filename, file_start, file_end);
          DEBUGMSG("Input image sequence %s\n", filename);

        }

      delete [] filenametmp;
    }
  else
    {

      int camera = -1;
      if (strlen(name) > 1)
        camera = atoi(&(name[1]));

      DEBUGMSG("Input camera #%d\n", camera);
      sequence = new CameraSequence(camera);

    }

  return sequence;
}


}

}