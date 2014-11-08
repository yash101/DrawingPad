#include <iostream>
#include <thread>
#include <string>
#include "include/server.hxx"
#include "include/Terminal.hxx"
#include "include/definitions.hxx"

#include <sys/types.h>
#include <unistd.h>

using namespace std;

void boot()
{
    //Just clear the screen if we are on a Mac. This is my dev PC, and whenever I do a terminal build,
    //there's a ton of crap on the screen!
#ifdef __MACH__
    std::system("clear");
#endif

#ifdef FORK
    std::ifstream fin("dat/pid.dat");
    if(fin.is_open())
    {
        fin << getpid();
    }
#endif

    paint_server serv;
    serv.set_listening_port(1234);
    std::cout << "PaintServer Started...!" << std::endl;
    serv.start_async();
#ifndef FORK
    std::thread terminal_thread(&terminal);
    terminal_thread.detach();
#endif
    while(true) std::this_thread::sleep_for(std::chrono::seconds(100));
}

int main()
{
#ifdef FORK
    pid_t p = fork();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::cout << "PID: " << p << std::endl;
    if(p < 0)
    {
        std::cout << "Failed to fork! Will run normally!" << std::endl;
    }
    else
    {
        if(p == 0)
        {
            std::cout << "Child has been spawned!" << std::endl;
        }
        else
        {
            std::cout << "Parent will now exit!" << std::endl;
            return 0;
        }
    }
#endif

    boot();
}
