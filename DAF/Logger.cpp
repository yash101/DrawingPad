/*
 * Logger.cpp
 *
 *  Created on: Nov 7, 2015
 *      Author: yash
 */

#include "Logger.h"
#include "AutoMutex.h"

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>

//Constructos
tool::Logger::Logger() :
  _file_location("")
{
}

tool::Logger::Logger(const char* location) :
  _file_location(location)
{
  this->open(_file_location);
}

tool::Logger::Logger(std::string location) :
  _file_location(location)
{
  this->open(_file_location);
}

tool::Logger::~Logger()
{
  _file.close();
}

//Opens a log file
void tool::Logger::open(std::string location)
{
  volatile tool::AutoMutex<std::mutex>(_sync_lock);
  if(!_file.is_open())
    _file.open(location.c_str(), std::ios::out | std::ios::app);
}

const char* tool::Logger::operator()(const char* message)
{
  volatile tool::AutoMutex<std::mutex>(_sync_lock);
  if(_file.is_open())
  {
    auto now = std::chrono::system_clock::now();
    auto tmt = std::chrono::system_clock::to_time_t(now);

    _file << "[" << std::put_time(std::gmtime(&tmt), "%Y-%m-%d %X GMT") << "]: \"" << message << "\"" << std::endl;
  }

  return message;
}

//Log a message
std::string& tool::Logger::operator()(std::string& message)
{
  volatile tool::AutoMutex<std::mutex>(_sync_lock);
  if(_file.is_open())
  {
    auto now = std::chrono::system_clock::now();
    auto tmt = std::chrono::system_clock::to_time_t(now);

    _file << "[" << std::put_time(std::gmtime(&tmt), "%Y-%m-%d %X GMT") << "]: \"" << message << "\"" << std::endl;
  }
  return message;
}

const char* tool::Logger::log(const char* message)
{
  volatile tool::AutoMutex<std::mutex>(_sync_lock);
  if(_file.is_open())
  {
    auto now = std::chrono::system_clock::now();
    auto tmt = std::chrono::system_clock::to_time_t(now);

    _file << "[" << std::put_time(std::gmtime(&tmt), "%Y-%m-%d %X GMT") << "]: \"" << message << "\"" << std::endl;
  }
  return message;
}

std::string& tool::Logger::log(std::string& message)
{
  volatile tool::AutoMutex<std::mutex>(_sync_lock);
  if(_file.is_open())
  {
    auto now = std::chrono::system_clock::now();
    auto tmt = std::chrono::system_clock::to_time_t(now);

    _file << "[" << std::put_time(std::gmtime(&tmt), "%Y-%m-%d %X GMT") << "]: \"" << message << "\"" << std::endl;
  }
  return message;
}

//Writes binary data
void* tool::Logger::write_bin(void* data, size_t len)
{
  volatile tool::AutoMutex<std::mutex>(_sync_lock);
  if(_file.is_open())
  {
    auto now = std::chrono::system_clock::now();
    auto tmt = std::chrono::system_clock::to_time_t(now);

    _file << "[" << std::put_time(std::gmtime(&tmt), "%Y-%m-%d %X GMT") << "]:\n{";
    _file.write((char*) data, len);
    _file << "}\n";
  }
  return data;
}

std::vector<char>& tool::Logger::write_bin(std::vector<char>& data)
{
  volatile tool::AutoMutex<std::mutex>(_sync_lock);
  if(_file.is_open())
  {
    auto now = std::chrono::system_clock::now();
    auto tmt = std::chrono::system_clock::to_time_t(now);

    _file << "[" << std::put_time(std::gmtime(&tmt), "%Y-%m-%d %X GMT") << "]:\n{";
    _file.write(data.data(), data.size() * sizeof(char));
    _file << "}\n";
  }
  return data;
}

//Writes hexadecimal data
void* tool::Logger::write_hex(void* data, size_t len)
{
  volatile tool::AutoMutex<std::mutex>(_sync_lock);
  if(_file.is_open())
  {
    auto now = std::chrono::system_clock::now();
    auto tmt = std::chrono::system_clock::to_time_t(now);

    _file << "[" << std::put_time(std::gmtime(&tmt), "%Y-%m-%d %X GMT") << "]:\n{\n";

    char* w = (char*) data;
    char buffer[512];
    size_t nrows = len / 16;
    size_t next = len % 16;

    for(size_t i = 0; i < nrows; i++)
    {
      sprintf(
        buffer,
        "\t[%u]\t%20hhX %20hhX %20hhX %20hhX %20hhX %20hhX %20hhX %20hhX %20hhX %20hhX %20hhX %20hhX %20hhX %20hhX %20hhX %20hhX",
        (int) i + 1,
        *((char*) w + sizeof(char) * 0),
        *((char*) w + sizeof(char) * 1),
        *((char*) w + sizeof(char) * 2),
        *((char*) w + sizeof(char) * 3),
        *((char*) w + sizeof(char) * 4),
        *((char*) w + sizeof(char) * 5),
        *((char*) w + sizeof(char) * 6),
        *((char*) w + sizeof(char) * 7),
        *((char*) w + sizeof(char) * 8),
        *((char*) w + sizeof(char) * 9),
        *((char*) w + sizeof(char) * 10),
        *((char*) w + sizeof(char) * 11),
        *((char*) w + sizeof(char) * 12),
        *((char*) w + sizeof(char) * 13),
        *((char*) w + sizeof(char) * 14),
        *((char*) w + sizeof(char) * 15)
      );

      _file << buffer << std::endl;

      w += 16 * sizeof(char);
    }

    if(next != 0)
    {
      _file << "\t[" << nrows + 1 << "]\t";
      for(size_t i = 0; i < next; i++)
      {
        sprintf(buffer, "%hhX ", *((char*) w + sizeof(char) * 1));
        _file << buffer;
      }

      _file << std::endl << "}" << std::endl;
    }
  }

  return data;
}

//Writes hex from a vector. Unimplemended
std::vector<char>& tool::Logger::write_hex(std::vector<char>& data)
{
  volatile tool::AutoMutex<std::mutex>(_sync_lock);
  return data;
}
