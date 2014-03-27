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

#include "common/platform/filesystem.h"

int read_file(const char* filename, char** buffer)
{

  FILE *fp = fopen(filename, "r");
  if (fp != NULL)
    {
      /* Go to the end of the file. */
      if (fseek(fp, 0L, SEEK_END) == 0)
        {
          /* Get the size of the file. */
          long bufsize = ftell(fp);
          if (bufsize == -1)
            {
              /* Error */
            }

          /* Allocate our buffer to that size. */
          *buffer = (char*) malloc(sizeof(char) * (bufsize));

          /* Go back to the start of the file. */
          if (fseek(fp, 0L, SEEK_SET) == 0)
            {
              /* Error */
            }

          /* Read the entire file into memory. */
          size_t newLen = fread(*buffer, sizeof(char), bufsize, fp);
          if (newLen == 0)
            {
              free(*buffer);
              return -1;
            }
        }
      fclose(fp);
    }

}

//free(source); /* Don't forget to call free() later! */