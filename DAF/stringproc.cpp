/*
 * stringproc.cpp
 *
 *  Created on: Nov 11, 2015
 *      Author: yash
 */

#include "stringproc.h"

#include <string.h>
#include <regex>
#include <cstring>

bool operator==(std::string a, std::string b)
{
  return !strcmp(a.c_str(), b.c_str());
}

bool operator!=(std::string a, std::string b)
{
  return strcmp(a.c_str(), b.c_str());
}

ssize_t daf::find(std::string in, char f)
{
  for(size_t i = 0; i < in.size(); i++)
  {
    if(in[i] == f) return (ssize_t) i;
  }

  return (ssize_t) -1;
}

ssize_t daf::find(std::string in, std::string f)
{
  if(in.size() < f.size()) return -1;
  size_t x = in.find(f);
  return ((x == std::string::npos) ? -1 : x);
}

template<typename T> void daf::fromString(T& x, std::string in)
{
  std::stringstream str(in);
  str >> x;
  return;
}

bool daf::contains(char find, std::string dict)
{
  for(size_t i = 0; i < dict.size(); i++)
  {
    if(find == dict[i]) return true;
  }
  return false;
}

static bool iispace(char ch)
{
  return isspace(ch) || ch == '\r' || ch == '\n';
}

std::string daf::trim(std::string str)
{
  return daf::ltrim(daf::rtrim(str));
}

std::string daf::ltrim(std::string in)
{
  std::string out = in;

  auto it = std::find_if(
    out.begin(),
    out.end(),
    [](char ch) { return !iispace(ch); }
  );

  out.erase(out.begin(), it);
  return out;
}


std::string daf::rtrim(std::string in)
{
  std::string str = in;
  auto it = std::find_if(
    str.rbegin(),
    str.rend(),
    [](char ch) { return !iispace(ch); }
  );

  str.erase(it.base() , str.end());

  return str;
}

void daf::itrim(std::string& in)
{
  in = daf::trim(in);
}

void daf::iltrim(std::string& in)
{
  in = daf::ltrim(in);
}

void daf::irtrim(std::string& in)
{
  in = daf::rtrim(in);
}

std::string daf::tolower(std::string in)
{
  std::string out;
  out.reserve(in.size());
  for(size_t i = 0; i < in.size(); i++)
  {
    out.push_back((char) ::tolower(in[i]));
  }
  return out;
}

std::string daf::toupper(std::string in)
{
  std::string out;
  out.reserve(in.size());
  for(size_t i = 0; i < in.size(); i++)
  {
    out.push_back((char) ::tolower(in[i]));
  }
  return out;
}

void daf::itolower(std::string& in)
{
  for(size_t i = 0; i < in.size(); i++)
  {
    in[i] = (char) ::tolower(in[i]);
  }
}

void daf::itoupper(std::string& in)
{
  for(size_t i = 0; i < in.size(); i++)
  {
    in[i] = (char) ::toupper(in[i]);
  }
}

void daf::tolowerf(char* str)
{
  char* ch = str;
  while(*ch != '\0')
  {
    *ch = (char) ::tolower(*str);
    ch += sizeof(char);
  }
}

void daf::toupperf(char* str)
{
  char* ch = (char*) str;
  while(*ch != '\0')
  {
    *ch = (char) ::tolower(*str);
    ch += sizeof(char);
  }
}

bool daf::getline(std::string& buffer, std::string end, std::istream& stream)
{
  buffer.clear();
  std::string out;
  while(stream.good())
  {
    if(out.size() >= end.size())
    {
      if(out.substr(out.size() - end.size(), out.size()) == end)
      {
        buffer = out.substr(0, out.size() - end.size());
        return true;
      }
    }
    out += (char) stream.get();
  }
  return false;
}

std::vector<std::string> daf::split(std::string stream, char f)
{
  if(stream.size() == 0) return std::vector<std::string>();
  std::string str = stream;
  std::vector<std::string> out;
  ssize_t occurrence = 0;

  while((occurrence = daf::find(str, f)) >= 0)
  {
    out.push_back(str.substr(0, occurrence));
    str = str.substr(occurrence + 1, str.size());
  }

  out.push_back(str);
  return out;
}

std::vector<std::string> daf::split(std::string stream, std::string f)
{
  std::string str = stream;
  std::vector<std::string> out;

  long occurrence = 0;

  while((occurrence = daf::find(str, f)) >= 0)
  {
    out.push_back(str.substr(0, occurrence));
    str = str.substr(occurrence + f.size(), str.size());
  }

  if(str.size() != 0)
    out.push_back(str);

  return out;
}
