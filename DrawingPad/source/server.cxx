#include "../include/server.hxx"
#include "../include/RAMFS.hxx"

//To compare two strings to each other. Note that this function is private to only this source file
inline bool operator== (std::string& a, std::string& b)
{
    return !strcmp(a.c_str(), b.c_str());
}

//Same as above. Just short circuits the conversion, from char* to std::string and back to char*. Just for optimization ;)
inline bool operator== (std::string& a, char* b)
{
    return !strcmp(a.c_str(), b);
}

//This is the function that will process each request. It will decide what it needs to do! ;)
const std::string paint_server::process_request(const dlib::incoming_things& incoming, dlib::outgoing_things& outgoing)
{
    if(incoming.path == "/keepalive")
    {
        sessions.keepAlive(incoming.queries["ses"]);
        return "Successful!";
    }
    //Note that I used an operator overload for std::strings. This is so that we can avoid excessive !strcmp()! ;)
    if(incoming.path == "/presenter")
    {
        return ramfs::filesystem.get_file_autocache("res/presenter.dev");
    }

    if(incoming.path == "/style.css")
    {
        outgoing.headers["Content-type"] = "text/css";
        return ramfs::filesystem.get_file_autocache("res/main.css");
    }

    if(incoming.path == "/presenter.js")
    {
        outgoing.headers["Content-type"] = "text/javascript";
        return ramfs::filesystem.get_file_autocache("res/presenter.js");
    }

    if(incoming.path == "/jquery.js")
    {
        outgoing.headers["Content-type"] = "text/javascript";
        return ramfs::filesystem.get_file_autocache("res/jquery.js");
    }

    if(incoming.path == "/")
    {
        return ramfs::filesystem.get_file_autocache("res/index.dev");
    }

    if(incoming.path == "/index.css")
    {
        outgoing.headers["Content-type"] = "text/css";
        return ramfs::filesystem.get_file_autocache("res/index.css");
    }

    //Viewer
    if(incoming.path == "/viewer")
    {
        std::stringstream str;
        str << "<script type=\"text/javascript\">" << std::endl;
        str << "var pid = \"\";" << std::endl;
        str << "</script>" << std::endl;
        str << ramfs::filesystem.get_file_autocache("res/viewer.dev");
        return str.str();
    }

    if(incoming.path == "/viewer.js")
    {
        outgoing.headers["Content-type"] = "text/javascript";
        return ramfs::filesystem.get_file_autocache("res/viewer.js");
    }

    if(incoming.path == "/fetch")
    {
        return sessions.getQueue(incoming.queries["ses"], atol(incoming.queries["index"].c_str()), false);
    }

    if(incoming.path == "/numqueues")
    {
        return dlib::cast_to_string(sessions.getQueueDepth(incoming.queries["ses"]));
    }

    if(incoming.path == "/project")
    {
        sessions.dequeue(incoming.queries["ses"], incoming.queries["req"]);
        if(DEBUG)
        {
            std::cout << "Queue: " << sessions.getQueue(incoming.queries["ses"], 0) << std::endl;
            std::cout << "NUMQUEUES: " << sessions.getQueueDepth(incoming.queries["ses"]) << std::endl;
            std::cout << "Done parsing response!" << std::endl;
        }
        return "Success!";
    }

    if(incoming.path == "/instructions")
    {
        if(incoming.queries["i"] == "viewer")
        {
            return ramfs::filesystem.get_file_autocache("res/instructions_viewer.dev");
        }
        if(incoming.queries["i"] == "presenter")
        {
            return ramfs::filesystem.get_file_autocache("res/instructions_presenter.dev");
        }
    }

    if(incoming.path == "/colorpicker")
    {
        outgoing.headers["Content-type"] = "text/plain";
        return ramfs::filesystem.get_file_autocache("res/colorpicker.mdev");
    }

    if(incoming.path.substr(0, 5) == "/r?i=")
    {
        std::stringstream str;
        str << "<script type=\"text/javascript\">" << std::endl;
        str << "var pid = \"" << incoming.path.substr(5, incoming.path.size()) << "\";" << std::endl;
        str << "</script>" << std::endl;
        str << ramfs::filesystem.get_file_autocache("res/viewer.dev");
        return str.str();
    }

    if(incoming.path == "/cursor.png")
    {
        outgoing.headers["Content-type"] = "image/png";
        return ramfs::filesystem.get_file_autocache("res/cursor.png");
    }

    if(incoming.path == "/cursor_canvas.png")
    {
        outgoing.headers["Content-type"] = "image/png";
        return ramfs::filesystem.get_file_autocache("res/cursor_canvas.png");
    }

    if(incoming.path == "/viewer.png")
    {
        outgoing.headers["Content-type"] = "image/png";
        return ramfs::filesystem.get_file_autocache("res/viewer.png");
    }
    if(incoming.path == "/presenter.png")
    {
        outgoing.headers["Content-type"] = "image/png";
        return ramfs::filesystem.get_file_autocache("res/presenter.png");
    }

    if(incoming.path == "/netcon.png")
    {
        outgoing.headers["Content-type"] = "image/png";
        return ramfs::filesystem.get_file_autocache("res/server_connected.png");
    }

    if(incoming.path == "/halt")
    {
        std::thread([](){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            raise(SIGINT);
            //exit(EXIT_SUCCESS);
        }).detach();
    }

    //Uh Oh! Error 404 :(
    outgoing.http_return = 404;
    return "Sorry! We could not load the resource! Please check the spelling of the domain :(";
}

const std::string paint_server::on_request(const dlib::incoming_things &incoming, dlib::outgoing_things &outgoing)
{
    if(DEBUG)
    {
        std::cout << "IP: " << incoming.foreign_ip << std::endl;
        std::cout << "PORT: " << incoming.foreign_port << std::endl;
        std::cout << "PATH: " << incoming.path << std::endl;
        std::cout << "REQUEST TYPE: " << incoming.protocol << std::endl;
        std::cout << "INPUT SIZE: " << incoming.queries["req"].size() << std::endl;
    }
    //^^ Let's just print out some connection details! ;)
    //WARNING: DISABLE THIS BEFORE PRODUCTION USE! WILL CAUSE A TON OF SHELL I/O :(
    //Perform the actual request and store it in a string!
    std::string request = process_request(incoming, outgoing);
    return request;
}
