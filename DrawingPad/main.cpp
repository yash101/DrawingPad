#include <iostream>
#include <thread>
#include <string>
#include "include/server.hxx"
#include "include/Terminal.hxx"
#include "include/definitions.hxx"

#include <sys/types.h>
#include <unistd.h>
using namespace std;

#ifdef SIGHANDLE
void sighandle(int sig)
{
    if(DEBUG)
    {
        std::cout << "Recieved signal: " << sig << std::endl;
    }
    if(sig == 1)
    {
        if(DEBUG)
        {
            std::cout << "SIGHUP sent. The user probably hung up. Let's continue running!" << std::endl;
        }
        return;
    }
    else if(sig == 2)
    {
        if(DEBUG)
        {
            std::cout << "SIGINT sent. We'll clean up our trash and exit! :)" << std::endl;
        }
    }
    else if(sig == SIGILL)
    {
        if(DEBUG)
        {
            std::cout << "SIGILL issued. There's something going wrong!" << std::endl;
        }
    }
    else if(sig == SIGABRT)
    {
        if(DEBUG)
        {
            std::cout << "SIGABRT issued. Something's going wrong! We're going to shut down!" << std::endl;
        }
    }
    else if(sig == SIGFPE)
    {
        if(DEBUG)
        {
            std::cout << "SIGFPE issued. Perhaps there is some sort of bug in the code that allows this program to divide by zero! Maybe I just ran out of bits!" << std::endl;
        }
    }
    else if(sig == SIGSEGV)
    {
        if(DEBUG)
        {
        std::cout << "SIGSEGV issued. SEGMENTATION FAULT!" << std::endl;
        }
    }
    else if(sig == SIGCHLD)
    {
        if(DEBUG)
        {
            std::cout << "Recieved SIGCHLD. I think we are being asked to terminate. Let's do it the proper way ;)" << std::endl;
        }
    }

    if(DEBUG)
    {
        std::cout << "Quitting!" << std::endl;
    }

    std::ofstream fout("dat/pid.dat");
    if(fout.is_open())
    {
        fout << "Done";
        fout.close();
    }

    if(sig == SIGSEGV)
    {
        abort();
    }

    exit(EXIT_SUCCESS);
}
#endif

void boot()
{
    //Set some signal handlers
#ifdef SIGHANDLE
    signal(SIGINT, sighandle);
    signal(SIGILL, sighandle);
    signal(SIGABRT, sighandle);
    signal(SIGFPE, sighandle);
    signal(SIGSEGV, sighandle);
    signal(SIGCHLD, sighandle);
#endif

    //Kill old children of same PID. Otherwise, we won't be able to open ports!
#ifdef AUTOKILL
    std::ifstream fin("dat/pid.dat");
    if(fin.is_open())
    {
        std::stringstream str;
        str << fin.rdbuf();
        if(strcmp(str.str().c_str(), "Done"))
        {
            if(DEBUG)
            {
                std::cout << "Killing Process with PID, " << str.str() << std::endl;
            }
            kill(atoi(str.str().c_str()), SIGINT);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        else
        {
            if(DEBUG)
            {
                std::cout << "Not going to kill! No old processes are running!" << std::endl;
            }
        }
        fin.close();
    }
#endif

    //Save the PID of the child
    std::fstream fout("dat/pid.dat");
    if(fout.is_open())
    {
        fout << getpid();
        fout.close();
    }

    //Start the server
    paint_server serv;
    serv.set_listening_port(1234);
    std::cout << "PaintServer Started...!" << std::endl;
    serv.start_async();

    //Start the terminal if we will not fork. Otherwise, std::cin is not the best thing to play around with!
#ifndef FORK
    if(TERMINAL)
    {
        std::thread terminal_thread(&terminal);
        terminal_thread.detach();
    }
#endif

    while(true) std::this_thread::sleep_for(std::chrono::seconds(100));
}

int main()
{

    //Just clear the screen if we are on a Mac. This is my dev PC, and whenever I do a terminal build,
    //there's a ton of crap on the screen!
#ifdef __MACH__
    std::system("clear");
#endif


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
            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::cout << "Parent will now exit!" << std::endl;
            return 0;
        }
    }
#endif

    boot();
}
