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

#ifndef _DEBUG_H
#define _DEBUG_H

#include "common/export.h"

#include <string>
#include <stdio.h>
#include <assert.h>

using namespace std;

#define SHORT_FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef BUILD_DEBUG

#define DEBUGGING if (__is_debug_enabled())

#define DEBUGMSG(...) if (__is_debug_enabled()) { fprintf(__debug_get_target(), "%s(%d): ", __short_file_name(__FILE__), __LINE__); fprintf(__debug_get_target(), __VA_ARGS__); }
#define PING if (__is_debug_enabled()) { fprintf(__debug_get_target(), "%s(%d): PING\n", __short_file_name(__FILE__), __LINE__); }


#else

#define DEBUGGING if (false)
#define DEBUGMSG(...)
#define PING

#endif

__LEGIT_EXPORT void __debug_enable();

__LEGIT_EXPORT void __debug_disable();

__LEGIT_EXPORT int __is_debug_enabled();

__LEGIT_EXPORT void __debug_set_target(FILE* stream);

__LEGIT_EXPORT FILE* __debug_get_target();

__LEGIT_EXPORT const char* __short_file_name(const char* filename);

__LEGIT_EXPORT void tic();

__LEGIT_EXPORT void toc();

class __LEGIT_EXPORT DebugOutput
{
public:
  DebugOutput(string dir);
  ~DebugOutput();

  void set_prefix(string p);

  string get_filename(string name);

private:

  string directory;

  string prefix;

};

#endif
