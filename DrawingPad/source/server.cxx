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
    //Note that I used an operator overload for std::strings. This is so that we can avoid excessive !strcmp()! ;)
    if(incoming.path == "/")
    {
        return ramfs::filesystem.get_file_autocache("res/index.dev");
    }

    if(incoming.path == "/style.css")
    {
        outgoing.headers["Content-type"] = "text/css";
        return ramfs::filesystem.get_file_autocache("res/main.css");
    }

    if(incoming.path == "/main.js")
    {
        outgoing.headers["Content-type"] = "text/javascript";
        return ramfs::filesystem.get_file_autocache("res/main.js");
    }

    if(incoming.path == "/jquery.js")
    {
        outgoing.headers["Content-type"] = "text/javascript";
        return ramfs::filesystem.get_file_autocache("res/jquery.js");
    }

    if(incoming.path == "/project")
    {
//        std::unordered_map<std::string, Session>::const_iterator it = sessions.find(incoming.queries["ses"]);
//        if(it == sessions.end())
//        {
//            sessions[incoming.queries["ses"]] = Session();
//        }

//        sessions[incoming.queries["ses"]].dequeue(incoming.queries["req"]);
//        std::cout << "Session Parser: " << sessions[incoming.queries["ses"]].getQueue(0) << std::endl;
//        std::cout << "Request served successfully!" << std::endl;
//        return "Your Request was Parsed Successfully!";
        sessions[incoming.queries["ses"]].dequeue(incoming.queries["req"]);
        std::cout << "Session: " << sessions[incoming.queries["ses"]].getQueue(0);
//        std::unordered_map<std::string, Session>::const_iterator it = sessions.find(incoming.queries["ses"]);
//        if(it == sessions.end())
//        {
//        }
//        else
//        {
//            Session ses = sessions[incoming.queries["req"]];
//            std::cout << "Session: " << ses.getQueue(0);
//            sessions[incoming.queries["ses"]] = ses;
//            sessions[incoming.queries["ses"]].
//        }
    }

    //Uh Oh! Error 404 :(
    outgoing.http_return = 404;
    return "Sorry! We could not load the resource! Please check the spelling of the domain :(";
}

const std::string paint_server::on_request(const dlib::incoming_things &incoming, dlib::outgoing_things &outgoing)
{
    //Let's just print out some connection details! ;)
    //WARNING: DISABLE THIS BEFORE PRODUCTION USE! WILL CAUSE A TON OF SHELL I/O :(
    //Lock the mutex. This is to grant 100% read/write accuracy and prevents memory corruption (a symptom of C++'s speed :D)
    locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT));
    //Perform the actual request and store it in a string!
    std::string request = process_request(incoming, outgoing);
    //Unlock the Mutex. FAIL TO DO THIS AND THE SERVER FREEZES AFTER THE FIRST CLIENT CONNECTION!
    locky_thingy.unlock();
    //Return the request back to the web server to be sent back!
    if(DEBUG)
    {
        std::cout << "IP: " << incoming.foreign_ip << std::endl;
        std::cout << "PORT: " << incoming.foreign_port << std::endl;
        std::cout << "PATH: " << incoming.path << std::endl;
        std::cout << "REQUEST TYPE: " << incoming.protocol << std::endl;
        std::cout << "INPUT SIZE: " << incoming.queries["req"].size() << std::endl;
    }
    return request;
}
