#include "../dlib/server.h"
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <string>
#include "../dlib/server/server_iostream.h"
#include "../dlib/base64.h"
#include "definitions.hxx"
#include <mutex>
#include <thread>
#include "Session.hxx"

class paint_server : public dlib::server_http
{
private:
    std::timed_mutex locky_thingy;
    std::unordered_map<std::string, std::string> db;
    std::unordered_map<std::string, Session> sessions;
    const std::string process_request(const dlib::incoming_things& incoming, dlib::outgoing_things& outgoing);
public:
    const std::string on_request(const dlib::incoming_things& incoming, dlib::outgoing_things& outgoing);
};
