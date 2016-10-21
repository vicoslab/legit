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
#ifndef mersenne_normal_twister_H
#define mersenne_normal_twister_H

#include "common/export.h"

//typedef unsigned long   uint32;

#ifdef PLATFORM_WINDOWS
#include "common/platform/windows/stdint_win.h"

#ifndef UINT32_C
#define UINT32_C(val) val##ui32
#endif

#else
#include <stdint.h>

#ifndef UINT32_C
#define UINT32_C(val) val##U
#endif

#endif

#include <stdio.h>

//#include <stdint.h>
//#include <inttypes.h>

//#include "pstdint.h"

#define TINYMT32_MEXP 127
#define TINYMT32_SH0 1
#define TINYMT32_SH1 10
#define TINYMT32_SH8 8
#define TINYMT32_MASK UINT32_C(0x7fffffff)
#define TINYMT32_MUL (1.0f / 4294967296.0f)

typedef unsigned long   uint32;

/**
 * tinymt32 internal state vector and parameters
 */
struct TINYMT32_T
{
  uint32_t status[4];
  uint32_t mat1;
  uint32_t mat2;
  uint32_t tmat;
};

typedef struct TINYMT32_T tinymt32_t;

//tinymt32_t random_sampler ;

void tinymt32_init(tinymt32_t * random, uint32_t seed);
void tinymt32_init_by_array(tinymt32_t * random, uint32_t init_key[],
                            int key_length);


__LEGIT_EXPORT float random_MT_uniform ( void ) ;
__LEGIT_EXPORT void set_seedMT ( uint32 seed ) ;
__LEGIT_EXPORT void initializeMTwister ( void ) ;
__LEGIT_EXPORT double random_MT_normal() ;


#if defined(__GNUC__)
/**
 * This function always returns 127
 * @param random not used
 * @return always 127
 */
inline static int tinymt32_get_mexp(
  tinymt32_t * random  __attribute__((unused)))
{
  return TINYMT32_MEXP;
}
#else
inline static int tinymt32_get_mexp(tinymt32_t * random)
{
  return TINYMT32_MEXP;
}
#endif

/**
 * This function changes internal state of tinymt32.
 * Users should not call this function directly.
 * @param random tinymt internal status
 */
inline static void tinymt32_next_state(tinymt32_t * random)
{
  uint32_t x;
  uint32_t y;

  y = random->status[3];
  x = (random->status[0] & TINYMT32_MASK)
      ^ random->status[1]
      ^ random->status[2];
  x ^= (x << TINYMT32_SH0);
  y ^= (y >> TINYMT32_SH0) ^ x;
  random->status[0] = random->status[1];
  random->status[1] = random->status[2];
  random->status[2] = x ^ (y << TINYMT32_SH1);
  random->status[3] = y;
  random->status[1] ^= -((int32_t)(y & 1)) & random->mat1;
  random->status[2] ^= -((int32_t)(y & 1)) & random->mat2;
}

/**
 * This function outputs 32-bit unsigned integer from internal state.
 * Users should not call this function directly.
 * @param random tinymt internal status
 * @return 32-bit unsigned pseudorandom number
 */
inline static uint32_t tinymt32_temper(tinymt32_t * random)
{
  uint32_t t0, t1;
  t0 = random->status[3];
#if defined(LINEARITY_CHECK)
  t1 = random->status[0]
       ^ (random->status[2] >> TINYMT32_SH8);
#else
  t1 = random->status[0]
       + (random->status[2] >> TINYMT32_SH8);
#endif
  t0 ^= t1;
  t0 ^= -((int32_t)(t1 & 1)) & random->tmat;
  return t0;
}

/**
 * This function outputs floating point number from internal state.
 * Users should not call this function directly.
 * @param random tinymt internal status
 * @return floating point number r (1.0 <= r < 2.0)
 */
inline static float tinymt32_temper_conv(tinymt32_t * random)
{
  uint32_t t0, t1;
  union
  {
    uint32_t u;
    float f;
  } conv;

  t0 = random->status[3];
#if defined(LINEARITY_CHECK)
  t1 = random->status[0]
       ^ (random->status[2] >> TINYMT32_SH8);
#else
  t1 = random->status[0]
       + (random->status[2] >> TINYMT32_SH8);
#endif
  t0 ^= t1;
  conv.u = ((t0 ^ (-((int32_t)(t1 & 1)) & random->tmat)) >> 9)
           | UINT32_C(0x3f800000);
  return conv.f;
}

/**
 * This function outputs floating point number from internal state.
 * Users should not call this function directly.
 * @param random tinymt internal status
 * @return floating point number r (1.0 < r < 2.0)
 */
inline static float tinymt32_temper_conv_open(tinymt32_t * random)
{
  uint32_t t0, t1;
  union
  {
    uint32_t u;
    float f;
  } conv;

  t0 = random->status[3];
#if defined(LINEARITY_CHECK)
  t1 = random->status[0]
       ^ (random->status[2] >> TINYMT32_SH8);
#else
  t1 = random->status[0]
       + (random->status[2] >> TINYMT32_SH8);
#endif
  t0 ^= t1;
  conv.u = ((t0 ^ (-((int32_t)(t1 & 1)) & random->tmat)) >> 9)
           | UINT32_C(0x3f800001);
  return conv.f;
}

/**
 * This function outputs 32-bit unsigned integer from internal state.
 * @param random tinymt internal status
 * @return 32-bit unsigned integer r (0 <= r < 2^32)
 */
inline static uint32_t tinymt32_generate_uint32(tinymt32_t * random)
{
  tinymt32_next_state(random);
  return tinymt32_temper(random);
}

/**
 * This function outputs floating point number from internal state.
 * This function is implemented using multiplying by 1 / 2^32.
 * floating point multiplication is faster than using union trick in
 * my Intel CPU.
 * @param random tinymt internal status
 * @return floating point number r (0.0 <= r < 1.0)
 */
inline static float tinymt32_generate_float(tinymt32_t * random)
{
  tinymt32_next_state(random);
  return tinymt32_temper(random) * TINYMT32_MUL;
}

/**
 * This function outputs floating point number from internal state.
 * This function is implemented using union trick.
 * @param random tinymt internal status
 * @return floating point number r (1.0 <= r < 2.0)
 */
inline static float tinymt32_generate_float12(tinymt32_t * random)
{
  tinymt32_next_state(random);
  return tinymt32_temper_conv(random);
}

/**
 * This function outputs floating point number from internal state.
 * This function is implemented using union trick.
 * @param random tinymt internal status
 * @return floating point number r (0.0 <= r < 1.0)
 */
inline static float tinymt32_generate_float01(tinymt32_t * random)
{
  tinymt32_next_state(random);
  return tinymt32_temper_conv(random) - 1.0f;
}

/**
 * This function outputs floating point number from internal state.
 * This function may return 1.0 and never returns 0.0.
 * @param random tinymt internal status
 * @return floating point number r (0.0 < r <= 1.0)
 */
inline static float tinymt32_generate_floatOC(tinymt32_t * random)
{
  tinymt32_next_state(random);
  return 1.0f - tinymt32_generate_float(random);
}

/**
 * This function outputs floating point number from internal state.
 * This function returns neither 0.0 nor 1.0.
 * @param random tinymt internal status
 * @return floating point number r (0.0 < r < 0.0)
 */
inline static float tinymt32_generate_floatOO(tinymt32_t * random)
{
  tinymt32_next_state(random);
  return tinymt32_temper_conv_open(random) - 1.0f;
}

/**
 * This function outputs double precision floating point number from
 * internal state. The returned value has 32-bit precision.
 * In other words, this function makes one double precision floating point
 * number from one 32-bit unsigned integer.
 * @param random tinymt internal status
 * @return floating point number r (1.0 < r < 2.0)
 */
inline static double tinymt32_generate_32double(tinymt32_t * random)
{
  tinymt32_next_state(random);
  return tinymt32_temper(random) * (1.0 / 4294967296.0);
}
#endif