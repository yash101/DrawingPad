#include "../include/Terminal.hxx"

//To compare two strings to each other. Note that this function is private to only this source file
inline bool operator== (std::string& a, std::string& b)
{
    return !strcmp(a.c_str(), b.c_str());
}

void terminal()
{
    while(true)
    {
        telnet_server telserv;
        telserv.set_listening_port(4321);
        //telserv.start_async();
        //std::cout << "Started Telnet Server!" << std::endl;
        std::string cmd;
        std::cout << "$ ";
        std::getline(std::cin, cmd);

        if(!strcmp(cmd.c_str(), "exit"))
        {
            exit(EXIT_SUCCESS);
        }

        if(!strcmp(cmd.c_str(), "terminate"))
        {
            std::terminate();
        }

        if(cmd == "recache")
        {
            std::cout << "Recaching all files!" << std::endl;
            ramfs::filesystem.sync();
        }

        std::cout << "+---------------------------------------------+" << std::endl;
        std::cout << "| Function not programed. Trying to shell it! |" << std::endl;
        std::cout << "+---------------------------------------------+" << std::endl;
        if(system(cmd.c_str()) != 0)
        {
            std::cout << "ERROR: [" + cmd + "] was a call to UNDEFINED" << std::endl;
        }
    }
}

void telnet_server::on_connect(std::istream &in, std::ostream &out, const std::string &foreign_ip, const std::string &local_ip, unsigned short foreign_port, unsigned short local_port, dlib::uint64 connection_id)
{
    std::string buf;
    out << "Please enter the passphrase!" << std::endl;
    out << "$ >> ";
    std::getline(in, buf);
    if(!strcmp(buf.c_str(), "password"))
    {
        out << "Authentication Successful!" << std::endl;
    }
    else
    {
        out << "Authentication Failure!\nClosing Connection." << std::endl;
        return;
    }
    while(true)
    {
        std::string cmd;
        out << "$ ";
        std::getline(in, cmd);

        if(!strcmp(cmd.c_str(), "exit"))
        {
            return;
        }

        out << "+---------------------------------------------+" << std::endl;
        out << "| Function not programed. Trying to shell it! |" << std::endl;
        out << "+---------------------------------------------+" << std::endl;
        if(system(cmd.c_str()) != 0)
        {
            out << "ERROR: [" + cmd + "] was a call to UNDEFINED" << std::endl;
        }
    }
}
