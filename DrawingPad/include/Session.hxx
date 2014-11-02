#ifndef SESSION_HXX
#define SESSION_HXX
#include <vector>
#include "Pixel.hxx"
#include <string>
#include <sstream>
#include <iostream>
#include "definitions.hxx"
#include <cstring>
#include <unordered_map>
#include <thread>
#include <mutex>
class SessionHost
{
private:
    std::unordered_map<std::string, std::vector<std::string>> data;
    std::unordered_map<std::string, long> timestamp;
    std::thread cron_thread;
    std::timed_mutex locky_thingy;
public:
    void dequeue(std::string sess, std::string queue);
    std::string getQueue(std::string sess, long index);
    std::string getQueue(std::string sess, long indl, bool null);
    long getQueueDepth(std::string sess);
    void cron();
    void keepAlive(std::string sess);
    SessionHost();
};
#endif // SESSION_HXX
