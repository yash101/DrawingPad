#ifndef TERMINAL_HXX
#define TERMINAL_HXX
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include "../dlib/server.h"

void terminal();
class telnet_server : public dlib::server_iostream
{
    void on_connect(std::istream &in, std::ostream &out, const std::string &foreign_ip, const std::string &local_ip, unsigned short foreign_port, unsigned short local_port, dlib::uint64 connection_id);
};

#endif // TERMINAL_HXX
