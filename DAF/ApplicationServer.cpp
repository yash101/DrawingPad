/*
 * ApplicationServer.cpp
 *
 *  Created on: Nov 18, 2015
 *      Author: yash
 */

#include "ApplicationServer.h"
#include "HttpServer.h"
#include "stringproc.h"
#include "entry.h"

#include <regex>
#include <iostream>

#include <sys/stat.h>

//Just some constructors. Can leave out. Probably should leave out,
//Make it possible for me to easily add extra stuff
daf::ApplicationServer::ApplicationServer()
{
}

daf::ApplicationServer::~ApplicationServer()
{
}

//This function handles requests
void daf::ApplicationServer::request_handler(daf::HttpServer::Session& session)
{
  //Check to see if we have an HTTP 1.0 request; If it is, use DefaultHostname
  if(session.Protocol == daf::Http::HTTP10)
  {
    if(!DefaultHostname(session))
    {
      throw daf::HttpServer::Exception("Could not resolve a hostname manager to handle the request!", -1, 500, __FILE__, __LINE__);
    }
  }
  else  //Find the Hostname to use
  {
    //This flag is true when we see success
    bool complete = false;

    //Try each Hostname to see if their regex matches what we are looking for
    for(std::map<std::string, daf::Hostname>::const_iterator it = Hostnames.begin(); it != Hostnames.end(); ++it)
    {
      //Try to match the regex
      if(std::regex_match(session.Path, std::regex(it->first)))
      {
        //Request the Hostname to pick a handler. If the hostname returns false, we try again later with
        //a new hostname
        complete = Hostnames[it->first](session);

        if(complete) break;
      }
    }

    //Check for completion. If we failed, we try the DefaultHostname
    if(!complete)
    {
      if(!DefaultHostname(session))
      {
        throw daf::HttpServer::Exception("Could not resolve a hostname manager to handle the request!", -1, 500, __FILE__, __LINE__);
      }
    }
  }
}

//Miscellaneous constructors/destructors
daf::Hostname::Hostname()
{
}

daf::Hostname::~Hostname()
{
}

//This function tries to choose a function to run
bool daf::Hostname::operator()(daf::HttpServer::Session& session)
{
  bool complete = false;

  //Iterate through all the pointer functions and run them if their regex matches Path
  for(std::map<std::string, daf::PointerFunction>::iterator it = _pfncs.begin(); it != _pfncs.end(); ++it)
  {
    if(std::regex_match(session.Path, std::regex(it->first)))
      complete = (it->second)(session);
    if(complete) break;
  }

  //Return if successful
  if(complete) return true;

  //Iterate through all the lambda function and run them if their regex matches Path
  for(std::map<std::string, daf::LambdaFunction>::iterator it = _lfncs.begin(); it != _lfncs.end(); ++it)
  {
    if(std::regex_match(session.Path, std::regex(it->first)))
      complete = (it->second)(session);
    if(complete) break;
  }

  //Return if successful
  if(complete) return true;

  //Iterate through all the pointer functions with data pointers and run them if their regex matches
  for(std::map<std::string, _dfunc>::const_iterator it = _dfncs.begin(); it != _dfncs.end(); ++it)
  {
    if(std::regex_match(session.Path, std::regex(it->first)))
      complete = (it->second).func(session, it->second.data);
    if(complete) break;
  }

  //Return the final status
  return complete;
}

//Checks if a mime exists. If it doesn't, it adds it
bool daf::pollMime(std::string extension, std::string fbm)
{
  if(Configuration()["ASHTTP.mime " + extension].size() == 0)
  {
    Configuration()["ASHTTP.mime " + extension] = fbm;
    Configuration().flush();
    return false;
  }
  else
  {
    return true;
  }
}

//Finds the mime to use from the main configuration file
//Adds an extra entry if it was not found
inline static std::string getHttpMime(std::string file_ext)
{
  std::string mime = Configuration()["ASHTTP.mime " + file_ext];
  if(mime.size() == 0)
  {
    Configuration()["ASHTTP.mime " + file_ext] = "octet/stream";
    Configuration().flush();
    mime = "octet/stream";
    std::cout << "Unable to find mime: " << file_ext << "! Adding as octet/stream!" << std::endl;
  }
  return mime;
}

//Gets the default indices from the main configuration file
inline static std::string getIndices()
{
  std::string ind = Configuration()["ASHTTP.indices"];
  if(ind.size() == 0)
  {
    Configuration()["ASHTTP.indices"] = "index.html,default.html,index.htm,default.htm";
    Configuration().flush();
    ind = Configuration()["ASHTTP.indices"];
  }
  return ind;
}

//This function is useful for handling static files
//data should be either `const char*` or `char*`
//data is the path which should be appended before the rest of the path
bool daf::static_handler(daf::HttpServer::Session& session, DPFDAT data)
{
  std::string upath;

  //Append the path prefix (from data) if it is defined (not NULL)
  if(data != NULL)
    upath = std::string((const char*) data) + session.Path;
  else
    upath = session.Path;

  //Split the path by the filesystem concatenator ('/')
  std::vector<std::string> parts = daf::split(upath, '/');

  //If the current path is a directory or not
  bool directory = false;

  //Final output path
  std::string fpath;
  //Used to check if a path is a dir or file or nonexistent
  struct stat st;

  for(size_t i = 0; i < parts.size(); i++)
  {
    //Skip if this could cause a vulnerability or if it doesn't help in any way
    if(parts[i].size() == 0 || parts[i] == "/" || parts[i] == "../" || parts[i] == "./" || parts[i] == ".")
      continue;

    //Append the filtered path part
    fpath += parts[i] + '/';

    //Check what this path is
    stat(fpath.substr(0, fpath.size() - 1).c_str(), &st);
    if(S_ISREG(st.st_mode))
    {
      directory = false;
      break;
    }
    else
    {
      directory = true;
    }
  }

  //Delete trailing ('/')
  if(fpath.size() > 0)
    fpath.pop_back();

  //If we hit a file
  if(!directory)
  {
    //Try to open the file
    if((session.Response.file = fopen(fpath.c_str(), "r")) != NULL)
    {
      //Set the response parameters
      session.Response.DataType = daf::Http::FILE;
      session.StatusCode = 200;

      //Separate out the file extension (for path)
      size_t pos = fpath.find_last_of('.');

      //Find and apply the proper mime type
      if(pos == std::string::npos || fpath.back() == '.')
      {
        session.Headers["content-type"] = "text/html";
      }
      else
      {
        session.Headers["content-type"] = getHttpMime(fpath.substr(pos + 1, fpath.size()));
      }

      return true;
    }
    else
    {
      //Return that we have a 404 error
      session.StatusCode = 404;
      session.Response.DataType = daf::Http::STRING;
      return false;
    }
  }
  //If we hit a directory
  else
  {
    //Get the indices and split them by the comma
    std::vector<std::string> indices = daf::split(getIndices(), ',');
    //Append a last ('/') if the path didn't end with one
    if(fpath.back() != '/') fpath.append("/");

    //Try each index
    for(size_t i = 0; i < indices.size(); i++)
    {
      std::string npath = fpath + indices[i];
      //Try the file
      if((session.Response.file = fopen(npath.c_str(), "r")) != NULL)
      {
        //Find the proper mime type
        session.Response.DataType = daf::Http::FILE;
        session.StatusCode = 200;

        size_t pos = indices[i].find_last_of('.');

        if(pos == std::string::npos || indices[i].back() == '.')
        {
          session.Headers["content-type"] = "text/html";
        }
        else
        {
          session.Headers["content-type"] = getHttpMime(indices[i].substr(pos + 1, indices[i].size()));
        }

        return true;
      }
    }

    return false;
  }

  return false;
}
