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

#ifndef LEGIT_DEFS
#define LEGIT_DEFS

#define COLOR_RED Scalar(0, 0, 255)
#define COLOR_GREEN Scalar(0, 255, 0)
#define COLOR_BLUE Scalar(255, 0, 0)
#define COLOR_BLACK Scalar(0, 0, 0)
#define COLOR_WHITE Scalar(255, 255, 255)
#define COLOR_YELLOW Scalar(0, 255, 255)

#define COLOR_GRAY(G) Scalar(G, G, G)

#ifdef WIN32
#define TEMP_DIR "C:\\Temp\\"
#else
#define TEMP_DIR "/tmp/"
#endif

#ifdef WIN32
#define STR_COPY(SRC,LEN,DST) strcpy_s(SRC,LEN,DST)
#define STR_PRINTF(BUF,LEN,PAT,...) sprintf_s(BUF,LEN,PAT,__VA_ARGS__)
#else
#define STR_COPY(SRC,LEN,DST) strcpy(SRC,DST)
#define STR_PRINTF(BUF,LEN,PAT,...) sprintf(BUF,PAT,__VA_ARGS__)
#endif

#if (CV_VERSION_EPOCH <= 2) && (CV_VERSION_MAJOR <= 4) && (CV_VERSION_MINOR < 9)
#define FILLED CV_FILLED
#define CAP_PROP_FRAME_COUNT CV_CAP_PROP_FRAME_COUNT
#define CAP_PROP_FRAME_WIDTH CV_CAP_PROP_FRAME_WIDTH
#define CAP_PROP_FRAME_HEIGHT CV_CAP_PROP_FRAME_HEIGHT
#define CAP_PROP_POS_FRAMES CV_CAP_PROP_POS_FRAMES
#define LINE_AA CV_AA
#endif

#if (CV_VERSION_EPOCH <= 2) && (CV_VERSION_MAJOR <= 3)
#define moveWindow cvMoveWindow
#endif

#endif