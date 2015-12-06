/*
 * Logger.h
 *
 *  Created on: Nov 7, 2015
 *      Author: yash
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>
#include <string>
#include <vector>
#include <mutex>

#ifdef _WIN32
typedef long long ssize_t;
#endif

namespace tool
{
  //Logs messages to a file
  class Logger
  {
  private:
    std::ofstream _file;
    std::string _file_location;
    std::mutex _sync_lock;

  public:
    Logger();
    Logger(const char* location);
    Logger(std::string location);

    void open(std::string location);

    const char* operator()(const char* message);
    std::string& operator()(std::string& message);

    const char* log(const char* message);
    std::string& log(std::string& message);

    void* write_bin(void* data, size_t len);
    std::vector<char>& write_bin(std::vector<char>& data);

    void* write_hex(void* data, size_t len);
    std::vector<char>& write_hex(std::vector<char>& data);

    size_t log_length();
    size_t log_clear();

    virtual ~Logger();
  };
}

#endif /* LOGGER_H_ */
