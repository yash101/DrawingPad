/*
 * Configuration.cpp
 *
 *  Created on: Nov 25, 2015
 *      Author: yash
 */

#include "Configuration.h"
#include <fstream>
#include <sstream>
#include <string.h>

#include "AutoMutex.h"
#include "stringproc.h"

//Initializers. Nothing big :)
daf::Config::Config() :
  FileLocation("")
{
  refresh();
}

daf::Config::Config(const char* location) :
  FileLocation(location)
{
  refresh();
}

daf::Config::Config(std::string location) :
  FileLocation(location)
{
  refresh();
}

//Accesses a stored configuration value
std::string& daf::Config::operator[](std::string key)
{
  return Storage[key];
}

//Returns a std::map<std::string, std::string> of the configuration data
const std::map<std::string, std::string> daf::Config::getMap()
{
  return Storage;
}

//Erases an element from the internal storage
bool daf::Config::remove(std::string key)
{
  try
  {
    Storage.erase(key);
    return true;
  }
  catch(std::exception& e)
  {
    return false;
  }
}

bool daf::Config::refresh()
{
  //Lock up for synchronization
  tool::AutoMutex<std::mutex>(Lock);
  std::string buffer;

  //Open the configuration file
  std::ifstream fin(FileLocation.c_str(), std::ios::in);

  if(!fin.is_open())
  {
    return false;
  }

  //Erase everything currently in the internal map
  Storage.clear();

  //Read lines from the file one by one
  while(std::getline(fin, buffer, '\n'))
  {
    //Trim the line
    daf::itrim(buffer);
    std::stringstream str;

    //Escape characters
    for(std::string::const_iterator it = buffer.begin(); it != buffer.end(); it++)
    {
      if((*it) == '\\')
      {
        if(it != buffer.end())
        {
          ++it;
          switch((*it))
          {
            case '\'':
              str << '\'';
              break;
            case 'n':
              str << '\n';
              break;
            case 'r':
              str << '\r';
              break;
            case 't':
              str << '\t';
              break;
            case '"':
              str << '"';
              break;

            default:
              str << '\\';
          }
        }
      }
      else
      {
        str << (*it);
      }
    }
    buffer = str.str();

    //Get rid of any comments
    size_t pos = buffer.find_last_of('#');
    if(pos != std::string::npos)
    {
      buffer = buffer.substr(0, pos);
    }

    //Trim the new line
    daf::itrim(buffer);
    if(buffer.size() == 0)
      continue;

    std::string key;
    std::string value;

    //Find the character which divides the key from the value
    ssize_t del = daf::find(buffer, ':');
    if(del < 0 || buffer.back() == ':')
    {
      if(buffer.back() == ':')
        buffer.pop_back();
      daf::itrim(buffer);
      key = buffer;
      value = "";
    }
    else
    {
      key = buffer.substr(0, del);
      value = buffer.substr(del + 1, buffer.size());

      daf::itrim(key);
      daf::itrim(value);
    }

    //Get rid of encompassing quotations
    if(value.back() == '\"' && value.front() == '\"')
      value = value.substr(1, value.size() - 2);
    if(key.back() == '\"' && key.front() == '\"')
      key = key.substr(1, key.size() - 2);

    //Save the key and value
    Storage[key] = value;
  }

  //Close the file
  fin.close();
  return true;
}

//Write the configuration file to a file.
bool daf::Config::flush()
{
  tool::AutoMutex<std::mutex>(Lock);
  FILE* fd = fopen(FileLocation.c_str(), "w");
  if(fd == NULL)
    return false;
  for(std::map<std::string, std::string>::const_iterator it = Storage.begin(); it != Storage.end(); ++it)
  {
    fprintf(fd, "\"%s\": \"%s\"\n", it->first.c_str(), it->second.c_str());
  }

  fclose(fd);
  return true;
}

std::string daf::Config::getFilename()
{
  return FileLocation;
}

void daf::Config::setFilename(std::string nloc)
{
  FileLocation = nloc;
}

daf::Config::~Config()
{}

//bool daf::Configuration::read()
//{
//  std::ifstream cf(location.c_str(), std::ios::in);
//
//  std::vector<std::string> namespace_stack;
//
//  //Random buffers used
//  ssize_t ss1;
//  char ch1;
//  std::string buffer;
//
//  while(true)
//  {
//    //Seek until we get something
//    while(cf.peek() != EOF && !isspace(cf.peek()))
//    {
//      ch1 = cf.get();
//    }
//
//    if((ch1 = cf.get()) != EOF)
//    {
//      if(ch1 == '}')
//      {
//        if(namespace_stack.size() != 0)
//          namespace_stack.pop_back();
//        else
//          return false;
//      }
//      else if(ch1 == '/')
//      {
//        if(cf.peek() == '/' || cf.peek() == '*')
//        {
//          if(cf.peek() == '/')
//          {
//            std::getline(cf, buffer, '\n');
//          }
//          else if(cf.peek() == '*')
//          {
//            while(true)
//            {
//              std::getline(cf, buffer, '*');
//              if(cf.get() == '/')
//                break;
//            }
//          }
//
//          continue;
//        }
//      }
//      else
//      {
//      }
//    }
//
//    std::getline(cf, buffer);
//    daf::itrim(buffer);
//    //Incrementing the namespace
//    if((ss1 = buffer.find("namespace") != std::string::npos))
//    {
//      buffer = buffer.substr(ss1 + strlen("namespace"), buffer.size());
//      daf::itrim(buffer);
//      while(cf.peek() != EOF && cf.peek() != '{')
//        ch1 = (char) cf.get();
//      namespace_stack.push_back(buffer);
//    }
//    else
//    {
//    }
//  }
//
//  return true;
//}
//
//daf::Configuration::Configuration()
//{}
//
//daf::Configuration::Configuration(const char* location)
//{}
//
//daf::Configuration::Configuration(std::string location)
//{}
//
//daf::Configuration::Configuration(daf::Configuration& configuration)
//{}
//
//daf::Configuration& daf::Configuration::operator=(daf::Configuration&& configuration)
//{
//  return (*this);
//}
