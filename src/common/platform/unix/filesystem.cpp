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


#include <fstream>
#include <errno.h>
#include <sys/stat.h>
#include "../filesystem.h"
#include "filesystem.h"

int file_type(const char* filename)
{

  struct stat buf;
  if (stat (filename, &buf) == -1 && errno == ENOENT)
    return FILETYPE_NONE;

  if (S_ISREG (buf.st_mode))
    {
      return FILETYPE_FILE;
    }
  if (S_ISDIR (buf.st_mode))
    {
      return FILETYPE_DIRECTORY;
    }

  return FILETYPE_OTHER;

}

string path_join(string root, string path)
{

  if (path[0] == '/')
    return path;
  else if (root[root.size()-1] == '/')
    return root + path;
  else return root + "/" + path;

}

string path_parent(string path)
{

  if (path.size() < 2)
    return path;

  int loc = path.rfind('/', (path[path.size()-1] == '/') ? path.size()-2 : string::npos);

  if (loc == string::npos)
    {
      return path;
    }
  else
    {
      return path.substr(0, loc);
    }

}