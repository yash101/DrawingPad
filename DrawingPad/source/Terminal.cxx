#include "../include/Terminal.hxx"

void terminal()
{
    while(true)
    {
        std::string cmd;
        std::cout << "$ ";
        std::getline(std::cin, cmd);

        std::cout << "+---------------------------------------------+" << std::endl;
        std::cout << "| Function not programed. Trying to shell it! |" << std::endl;
        std::cout << "+---------------------------------------------+" << std::endl;
        if(system(cmd.c_str()) != 0)
        {
            std::cout << "ERROR: [" + cmd + "] was a call to UNDEFINED" << std::endl;
        }
    }
}
