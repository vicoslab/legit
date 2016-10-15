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

#ifndef _COMMON_MATH_H
#define _COMMON_MATH_H

#include <math.h>

#ifdef PLATFORM_WINDOWS
#include "common/platform/windows/stdint_win.h"
#else
#include <stdint.h>
#endif

#define SQRT(X) sqrt(X)
#define SQRT_INV(X) (1.0 / sqrt(X))

#ifndef MAX
#define MAX(a,b) ((a) > (b)) ? (a) : (b)
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b)) ? (a) : (b)
#endif

#ifndef PI
#define PI  3.1415926f
#endif

#define CLAMP3(f,min,max)  ((f)<(min)?(min):(f)>(max)?(max):(f))

#ifdef BUILD_FAST_MATH
#include "common/math/mersenne.h"
#define RANDOM_SEED(S) set_seedMT (S)
#define RANDOM_UNIFORM (double) random_MT_uniform()
#define RANDOM_NORMAL random_MT_normal()
#else
#define RANDOM_SEED(S) srandom(S)
#define RANDOM_UNIFORM double(rand())/double(RAND_MAX)
#define RANDOM_NORMAL randn()
#endif

#endif