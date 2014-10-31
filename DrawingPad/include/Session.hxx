#ifndef SESSION_HXX
#define SESSION_HXX
#include <vector>
#include "Pixel.hxx"
#include <string>
#include <sstream>
#include <iostream>
#include "definitions.hxx"
#include <cstring>
class Session
{
private:
    std::vector<std::vector<Point> > image;
    std::stringstream queue_out;
    std::vector<std::string> queues;
public:
    void dequeue(std::string queue);
    void dequeue_new(std::string queue);
    std::string getqueue();
    std::string getQueue(long index);
    long getQueueDepth();
};
#endif // SESSION_HXX
