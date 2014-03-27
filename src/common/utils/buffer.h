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

#ifndef LEGIT_BUFFER
#define LEGIT_BUFFER

#include <iostream>
#include "debug.h"

using namespace std;

namespace legit
{

template <class T>
class Buffer
{
private:
  int start;
  int end;
  int _capacity;
  int _limit;
  bool full;
  bool empty;
  T *buffer;

public:
  /**
      capacity: current capacity
      limit:  max size
  */
  Buffer(int capacity, int limit = 0)
  {
    _capacity = capacity;
    _limit = limit;
    full = false;
    empty = true;
    buffer = new T[capacity];

//printf("ptr: %ld \n", (long int)buffer);
    start = -1;
    end = -1;
  }

  ~Buffer()
  {
    delete [] buffer;
  }

  void push(T value)
  {

    end++;
    if (end < _capacity)
      {
//DEBUGMSG("%d %d %d \n", end, start, size());
        buffer[end] = value;
        if (end == start)
          {
            start++;
            if (start == _capacity)
              start = 0;
          }
      }
    else
      {

        if (_capacity < _limit || _limit < 1)
          {
            int newcapacity = _limit < 1 ? _capacity + 100 : MIN(_limit, _capacity + 100);

            T* newbuffer = new T[newcapacity];

            for (int i = 0; i < _capacity; i++)
              {
                newbuffer[i] = buffer[i];
              }

            delete [] buffer;
            buffer = newbuffer;
//DEBUGMSG("Reallocated \n");
//                buffer = (T*) realloc(buffer, sizeof(T) * newcapacity);
            _capacity = newcapacity;
            buffer[end] = value;
          }
        else
          {
            full = true;
            end = 0;
            buffer[end] = value;
            start = 1;
          }
      }
  }

  void set(T value)
  {
    buffer[end] = value;
  }

  inline T get(int offset)
  {

    if (start > end)
      {
        if (_capacity < offset) throw exception();
        int o = (end - offset);
        if (o < 0) o += _capacity;
        return buffer[o];
      }
    else
      {
        if (end - start < offset) throw exception();
        int o = (end - offset);
        if (o < 0) throw exception();
        return buffer[o];
      }

  }

  inline T get()
  {

    if (start > end)
      {
        if (end < 0) return buffer[end + _capacity];
        else return buffer[end];
      }
    else
      {
        if (end < 0) throw exception();
        return buffer[end];
      }

  }

  inline T get_last()
  {

    int offset = ((start > end || full) ? (end == -1 ? 0 : _capacity) : end - start) - 1;

    if (start > end)
      {
        if (_capacity < offset) throw exception();
        int o = (end - offset);
        if (o < 0) o += _capacity;
        return buffer[o];
      }
    else
      {
        if (end - start < offset) throw exception();
        int o = (end - offset);
        if (o < 0) throw exception();
        return buffer[o];
      }

  }

  int size()
  {
    return (start > end || full) ? (end == -1 ? 0 : _capacity) : end - start;
  }

  bool is_full()
  {
    return full;
  }

  int capacity()
  {
    return _capacity;
  }

  int limit()
  {
    return _limit;
  }

  void flush()
  {
    start = 0;
    end = 0;
    full = false;
    empty = true;
  }

  T* array()
  {

    if (size() == 0)
      return NULL;

    T* result = malloc(sizeof(T) * size());

    int pos = start;
    int i = 0;
    while (pos != end)
      {
        result[i] = buffer[pos];

        pos++;
        i++;

        if (pos >= _capacity)
          {
            pos = 0;
          }
      }

    return result;
  }

};


}
#endif
