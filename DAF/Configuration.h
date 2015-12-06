/*
 * Configuration.h
 *
 *  Created on: Nov 25, 2015
 *      Author: yash
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
#include <string>
#include <vector>
#include <map>
#include <mutex>
namespace daf
{
  class Config
  {
  private:
    std::string FileLocation;
    std::mutex Lock;
    std::map<std::string, std::string> Storage;
  public:
    Config();
    Config(const char* loc);
    Config(std::string loc);

    virtual ~Config();

    std::string& operator[](std::string key);

    const std::map<std::string, std::string> getMap();
    bool remove(std::string key);

    bool refresh();
    bool flush();

    std::string getFilename();
    void setFilename(std::string nloc);
  };
//  typedef void* DPFDAT;
//
//  class Configuration;
//  class ConfigItem;
//
//  enum ConfigType
//  {
//    Namespace,
//    Number,
//    String,
//    Binary
//  };
//
//  enum NumberType
//  {
//    Integer,
//    Floating
//  };
//
//  class ConfigItem
//  {
//  public:
//    ConfigType type;
//    DPFDAT data;
//  };
//
//  class Configuration
//  {
//  private:
//    std::map<std::string, ConfigItem> ConfigItems;
//    std::string location;
//  public:
//    Configuration();
//    Configuration(const char* location);
//    Configuration(std::string location);
//    Configuration(Configuration& configuration);
//    Configuration& operator=(Configuration&& configuration);
//    virtual ~Configuration();
//
//    bool read();
//  };
}

#endif /* CONFIGURATION_H_ */
