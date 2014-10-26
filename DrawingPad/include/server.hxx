#include "dlib/server.h"
#include <string>
#include <sstream>
#include <fstream>
class paint_server : public dlib::server
{
    void on_connect(dlib::connection& connection);
};

