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

#include "common/utils/config.h"
#include "common/utils/utils.h"
#include "common/platform/filesystem.h"

using std::string;

namespace legit
{
namespace common
{

Config::Config( string filename, string delimiter, string comment, string special, string sentry )
  : myDelimiter(delimiter), myComment(comment), mySentry(sentry), mySpecial(special)
{
  // Construct a Config, getting keys and values from given file

  std::ifstream in( filename.c_str() );

  if( !in ) throw file_not_found( filename );

  in >> (*this);
}


Config::Config()
  : myDelimiter( string(1,'=') ), myComment( string(1,'#') ), mySpecial( string(1,'@') )
{
  // Construct a Config without a file; empty
}


void Config::remove( const string& key )
{
  // Remove key and its value
  myContents.erase( myContents.find( key ) );
  return;
}


bool Config::keyExists( const string& key ) const
{
  // Indicate whether key is found
  mapci p = myContents.find( key );
  return ( p != myContents.end() );
}


/* static */
void Config::trim( string& s )
{
  // Remove leading and trailing whitespace
  static const char whitespace[] = " \n\t\v\r\f";
  s.erase( 0, s.find_first_not_of(whitespace) );
  s.erase( s.find_last_not_of(whitespace) + 1U );
}

vector<string> Config::keys()
{

  vector<string> k;

  map<string,string>::iterator it;

  for ( it=myContents.begin() ; it != myContents.end(); it++ )
    k.push_back((*it).first);

  return k;
}

std::ostream& operator<<( std::ostream& os, const Config& cf )
{
  // Save a Config to os
  for( Config::mapci p = cf.myContents.begin();
       p != cf.myContents.end();
       ++p )
    {
      os << p->first << " " << cf.myDelimiter << " ";
      os << p->second << std::endl;
    }
  return os;
}


std::istream& operator>>( std::istream& is, Config& cf )
{
  // Load a Config from is

  cf.load_config(is, NULL);

  return is;

}

void Config::load_config(string file_name)
{

  ifstream in_file( file_name.c_str() , ifstream::in );
  string context = path_parent(file_name);

  if (in_file)
    {
      load_config(in_file, context.c_str());
      in_file.close();
    }

}

void Config::load_config(std::istream& is, const char* context_path)
{

  // Read in keys and values, keeping internal whitespace
  typedef string::size_type pos;
  const string& delim  = myDelimiter;  // separator
  const string& comm   = myComment;    // comment
  const string& sentry = mySentry;     // end of file sentry
  const string& special = mySpecial;     // end of file sentry
  const pos skip = delim.length();        // length of separator

  string nextline = "";  // might need to read ahead to see where value ends

  while( is || nextline.length() > 0 )
    {
      // Read an entire line at a time
      string line;
      if( nextline.length() > 0 )
        {
          line = nextline;  // we read ahead; use it now
          nextline = "";
        }
      else
        {
          std::getline( is, line );
        }
      // Ignore comments
      line = line.substr( 0, line.find(comm) );

      // Check for end of file sentry
      if( sentry != "" && line.find(sentry) != string::npos ) return;

      if( special != "" && line.find(special) == 0 && line.find(" ") != string::npos )
        {

          int splitPos = line.find(" ");
          string instruction = line.substr( 0, splitPos );
          string parameters = line.substr( splitPos+1, string::npos );
          Config::trim(parameters);

          if (instruction.compare("@include") == 0 && context_path)
            {

              string include_file = path_join(string(context_path), parameters);

              if (file_type(include_file.c_str()) != FILETYPE_FILE)
                {
                  DEBUGMSG("File not found '%s'\n", include_file.c_str());
                }
              else
                {

                  DEBUGMSG("Including config file '%s'\n", include_file.c_str());
                  ifstream in_file( include_file.c_str() , ifstream::in );

                  if (in_file)
                    {
                      string context = path_parent(include_file);
                      load_config(in_file, context.c_str());
                      in_file.close();
                    }

                }

            }

          else
            DEBUGMSG("Unknown config instruction '%s'\n", instruction.c_str());

          continue;
        }

      // Parse the line if it contains a delimiter
      pos delimPos = line.find( delim );
      if( delimPos < string::npos )
        {
          // Extract the key
          string key = line.substr( 0, delimPos );
          line.replace( 0, delimPos+skip, "" );

          // See if value continues on the next line
          // Stop at blank line, next line with a key, end of stream,
          // or end of file sentry
          bool terminate = false;
          while( !terminate && is )
            {
              std::getline( is, nextline );
              terminate = true;

              string nlcopy = nextline;
              Config::trim(nlcopy);
              if( nlcopy == "" ) continue;

              nextline = nextline.substr( 0, nextline.find(comm) );
              if( nextline.find(delim) != string::npos )
                continue;
              if( sentry != "" && nextline.find(sentry) != string::npos )
                continue;

              nlcopy = nextline;
              Config::trim(nlcopy);
              if( nlcopy != "" ) line += "\n";
              line += nextline;
              terminate = false;
            }

          // Store key and value
          Config::trim(key);
          Config::trim(line);
          myContents[key] = line;  // overwrites if key is repeated
        }
    }

}

}
}
