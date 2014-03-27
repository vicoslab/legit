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

#include <time.h>
#include <string.h>
#include <stdio.h>
#include "debug.h"
#include "common/platform/filesystem.h"

using namespace std;

long __start_time = clock();

#ifdef SOURCE_COMPILE_ROOT
const char* _source_root = SOURCE_COMPILE_ROOT;
#else
const char* _source_root = "";
#endif
const int _source_root_len = strlen(_source_root);

#ifdef LEGIT_ENABLE_DEBUG
int __legit_debug = 1;
#else
int __legit_debug = 0;
#endif

FILE* __legit_stream = stderr;

void __debug_enable()
{
  __legit_debug = 1;
}

void __debug_disable()
{
  __legit_debug = 0;
}

int __is_debug_enabled()
{
  return __legit_debug;
}

void __debug_set_target(FILE* stream)
{
  __legit_stream = stream;
}

FILE* __debug_get_target()
{
  return __legit_stream;
}

void tic()
{
  __start_time = clock();
}

void toc()
{
  long diff = clock() - __start_time;
  DEBUGMSG(" *** Elapsed time %ldms\n", (diff) / (CLOCKS_PER_SEC / 1000));
}

const char* __short_file_name(const char* filename)
{

  int position = 0;

  while (position < _source_root_len)
    {

      if (!filename[position])
        return filename;

      if (_source_root[position] != filename[position])
        return filename;

      position++;
    }

  return &(filename[position]);
}


DebugOutput::DebugOutput(string dir) : directory(dir), prefix("")
{

}

DebugOutput::~DebugOutput()
{

}

void DebugOutput::set_prefix(string p)
{

  prefix = p;

}

string DebugOutput::get_filename(string name)
{

  return path_join(directory, prefix + name);

}