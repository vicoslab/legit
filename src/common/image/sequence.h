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

#ifndef LEGIT_SEQUENCE
#define LEGIT_SEQUENCE

#include "common/export.h"

#include <stdio.h>
#include <string.h>
#include <exception>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "common/math/geometry.h"
#include "common/utils/utils.h"
#include "common/utils/defs.h"

using namespace cv;

namespace legit
{

namespace common
{


class __LEGIT_EXPORT Sequence
{
public:

  Sequence() {};

  virtual ~Sequence() {};

  virtual bool read_frame(Mat& img) = 0;

  virtual bool can_skip()
  {
    return false;
  }

  virtual bool is_finite() = 0;

  virtual int position() = 0;

  virtual int size() = 0;

  virtual int skip(int position) = 0;

  virtual int width() = 0;

  virtual int height() = 0;

};


class __LEGIT_EXPORT VideoFileSequence : public Sequence
{
public:

  VideoFileSequence(const char* file, int start=-1, int end=-1);

  virtual ~VideoFileSequence();

  virtual bool read_frame(Mat& img);

  virtual bool can_skip()
  {
    return true;
  }

  virtual bool is_finite()
  {
    return true;
  }

  virtual int position();

  virtual int size();

  virtual int skip(int position);

  virtual int width();

  virtual int height();

  const char* get_file();

protected:

  int file_start;

  int file_end;

  int current_frame;

  char* sequence_file;

  VideoCapture cap;

};

class __LEGIT_EXPORT CameraSequence : public Sequence
{
public:

  CameraSequence(int camera);

  virtual ~CameraSequence();

  virtual bool read_frame(Mat& img);

  virtual bool is_finite()
  {
    return false;
  };

  virtual int position();

  virtual int size();

  virtual int skip(int position);

  virtual int width();

  virtual int height();

protected:

  int current_frame;

  VideoCapture cap;

};

class __LEGIT_EXPORT ImageFileListSequence : public Sequence
{
public:

  ImageFileListSequence();

  virtual ~ImageFileListSequence();

  virtual bool read_frame(Mat& img);

  virtual bool is_finite()
  {
    return true;
  }

  virtual int position();

  virtual int size();

  virtual int skip(int position);

  virtual int width();

  virtual int height();

  void list_files();

protected:

  int file_start;

  int file_end;

  vector<string> files;

  int sequence_width;

  int sequence_height;

  int current_frame;

  virtual bool load_list();

};


class __LEGIT_EXPORT FileListSequence : public ImageFileListSequence
{
public:

  FileListSequence(const char* listing);

  virtual ~FileListSequence();

  const char* get_listing();

protected:

  string sequence_listing;

  bool read_listing(const string listing);

  virtual bool load_list();
};

class __LEGIT_EXPORT  ImageDirectorySequence : public ImageFileListSequence
{
public:

  ImageDirectorySequence(const char* directory, int start=-1, int end=-1);

  virtual ~ImageDirectorySequence();

  const char* get_directory();

protected:

  string sequence_source;

  string sequence_directory;

  bool scan_directory(const string& dir);

  virtual bool load_list();
};

__LEGIT_EXPORT Sequence* open_sequence(const char* name);


}

}

#endif
