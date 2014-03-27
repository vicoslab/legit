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


#include <string.h>
#include <string>
#include "string.h"

using namespace std;

bool matches_suffix(const char* str, const char* suffix)
{

  int len = strlen(str);
  int slen = strlen(suffix);

  if (slen > len)
    return false;

  for (int i = 1; i <= slen; i++)
    {
      if (str[len-i] != suffix[slen-i])
        return false;

    }

  return true;
}

bool matches_prefix(const char* str, const char* prefix)
{

  int len = strlen(str);
  int slen = strlen(prefix);

  if (slen > len)
    return false;

  for (int i = 0; i < slen; i++)
    {
      if (str[i] != prefix[i])
        return false;

    }

  return true;
}

bool matches_suffix(const string str, const string suffix)
{

  return matches_suffix(str.c_str(), suffix.c_str());

}

bool matches_prefix(const string str, const string prefix)
{

  return matches_prefix(str.c_str(), prefix.c_str());

}