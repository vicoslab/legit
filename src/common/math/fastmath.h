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

#ifndef _FAST_MATH_H
#define _FAST_MATH_H

/*
 *	Square root by abacus algorithm, Martin Guy @ UKC, June 1985.
 *	From a book on programming abaci by Mr C. Woo.
 *	Argument is a positive integer, as is result.
 *
 *	I have formally proved that on exit:
 *		   2		   2		   2
 *		res  <= x < (res+1)	and	res  + op == x
 *
 *	This is also nine times faster than the library routine (-lm).
 */

inline int intsqrt(int x)
{
  /*
   *	Logically, these are unsigned. We need the sign bit to test
   *	whether (op - res - one) underflowed.
   */

  register int op, res, one;

  op = x;
  res = 0;

  /* "one" starts at the highest power of four <= than the argument. */

  one = 1 << 30;	/* second-to-top bit set */
  while (one > op) one >>= 2;

  while (one != 0)
    {
      if (op >= res + one)
        {
          op = op - (res + one);
          res = res +  2 * one;
        }
      res /= 2;
      one /= 4;
    }
  return(res);
}

inline float am_fsqrt(float x)  // 0,00079 on sqrt(220.54) , t = 220, tref = 580
//http://www.codemaestro.com/reviews/review00000105.html
{
  float x_in = x ;
  float xhalf = 0.5f*x;
  int i = *(int*)&x; // get bits for floating value
  i = 0x5f375a86- (i>>1); // gives initial guess y0
  x = *(float*)&i; // convert bits back to float
  x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
  return x*x_in;
}


//---------------------------------------------------------------------------
inline float am_fsqrt2(float x)  // 6E-7 on sqrt(220.54) , t = 320, tref = 580
//http://www.codemaestro.com/reviews/review00000105.html
{
  float x_in = x ;
  float xhalf = 0.5f*x;
  int i = *(int*)&x; // get bits for floating value
  i = 0x5f375a86- (i>>1); // gives initial guess y0
  x = *(float*)&i; // convert bits back to float
  x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
  x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
  return x*x_in;
}

//---------------------------------------------------------------------------

inline float am_finvsqrt(float x)  // 0,00079 on sqrt(220.54)
//http://www.codemaestro.com/reviews/review00000105.html
{
  float xhalf = 0.5f*x;
  int i = *(int*)&x; // get bits for floating value
  i = 0x5f375a86- (i>>1); // gives initial guess y0
  x = *(float*)&i; // convert bits back to float
  x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
  x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
  return x;
}

//---------------------------------------------------------------------------
inline float am_finvsqrt2(float x)  // 6E-7 on sqrt(220.54)
//http://www.codemaestro.com/reviews/review00000105.html
{
  float xhalf = 0.5f*x;
  int i = *(int*)&x; // get bits for floating value
  i = 0x5f375a86- (i>>1); // gives initial guess y0
  x = *(float*)&i; // convert bits back to float
  x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
  x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
  return x;
}


//---------------------------------------------------------------------------
inline float am_froot(float f,int n) // 0.04 general root
//http://www.musicdsp.org/showone.php?id=133
{
  long *lp,l;
  lp=(long*)(&f);
  l=*lp;
  l-=0x3F800000l;
  l>>=(n-1);
  l+=0x3F800000l;
  *lp=l;
  return f;
}
//---------------------------------------------------------------------------


#endif