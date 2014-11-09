#include <iostream>
#include <thread>
#include <string>
#include "include/server.hxx"
#include "include/Terminal.hxx"
#include "include/definitions.hxx"

#include <sys/types.h>
#include <unistd.h>
#ifndef __WIN32
#include <dirent.h>
#include <sys/stat.h>
#else
#include <windows.h>
#endif
using namespace std;

//Before we boot...
void preboot()
{
    if(DEBUG)
    {
        std::cout << "Creating the data directory!" << std::endl;
    }
#ifdef __WIN32
    CreateDirectory("dat", NULL);
#else
    mkdir("dat", 0755);
#endif
}

#ifdef SIGHANDLE
void sighandle(int sig)
{
    std::ofstream tout("dat/log.d", std::ios::app);
    if(tout.is_open())
    {
        if(!DEBUG)
        {
            tout << "Recieved signal: " << sig << std::endl;
        }
        if(sig == 1)
        {
            if(!DEBUG)
            {
                tout << "SIGHUP sent. The user probably hung up. Let's continue running!" << std::endl;
            }
            return;
        }
        else if(sig == 2)
        {
            if(!DEBUG)
            {
                tout << "SIGINT sent. We'll clean up our trash and exit! :)" << std::endl;
            }
        }
        else if(sig == SIGILL)
        {
            if(!DEBUG)
            {
                tout << "SIGILL issued. There's something going wrong!" << std::endl;
            }
        }
        else if(sig == SIGABRT)
        {
            if(!DEBUG)
            {
                tout << "SIGABRT issued. Something's going wrong! We're going to shut down!" << std::endl;
            }
        }
        else if(sig == SIGFPE)
        {
            if(!DEBUG)
            {
                tout << "SIGFPE issued. Perhaps there is some sort of bug in the code that allows this program to divide by zero! Maybe I just ran out of bits!" << std::endl;
            }
        }
        else if(sig == SIGSEGV)
        {
            if(!DEBUG)
            {
                tout << "SIGSEGV issued. SEGMENTATION FAULT!" << std::endl;
            }
        }
        else if(sig == SIGCHLD)
        {
            if(!DEBUG)
            {
                tout << "Recieved SIGCHLD. I think we are being asked to terminate. Let's do it the proper way ;)" << std::endl;
            }
        }
        tout.close();
    }

    if(DEBUG)
    {
        std::cout << "Quitting!" << std::endl;
    }

    std::ofstream fout("dat/pid.dat");
    if(fout.is_open())
    {
        fout << "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
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
    if(fin.is_open() && fin.good())
    {
        std::stringstream str;
        str << fin.rdbuf();
        if(str.str().size() < 8 && str.str().size() > 0)
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
    if(!fin.good())
    {
        if(DEBUG)
        {
            std::cout << "PIDFILE does not exist!" << std::endl;
        }
    }
#endif

    //Save the PID of the child
    std::ofstream fout("dat/pid.dat");
    if(fout.is_open())
    {
        fout << getpid();
        fout.close();
    }

    if(DEBUG)
    {
        std::cout << "Launching the server!" << std::endl;
    }
//    try
//    {
        //Start the server
        paint_server serv;
        serv.set_listening_port(PORTNO_PAINTSERVER);
        serv.start_async();
//    }
//    catch(exception& e)
//    {
//        if(DEBUG)
//        {
//            std::cout << "Unable to start the server on port " << PORTNO_PAINTSERVER << std::endl;
//            std::cout << "Perhaps there is another server using port " << PORTNO_PAINTSERVER << std::endl;
//            std::cout << "Otherwise, it's also possible that either you disabled AUTOKILL, or AUTOKILL failed to run!" << std::endl;
//            std::cout << "Exception data: [" << e.what() << "]" << std::endl;
//            std::cout << "Quitting because of internal server failure!!!" << std::endl;
//            raise(SIGILL);
//        }
//        std::ofstream lout("dat/log.d");
//        if(lout.is_open())
//        {
//            fout << "Quitting! [" << e.what() << "]" << std::endl;
//            lout.close();
//        }
//    }
    if(DEBUG)
    {
        std::cout << "PaintServer Started...!" << std::endl;
    }

    //Start the terminal if we will not fork. Otherwise, std::cin is not the best thing to play around with!
#ifndef FORK
    std::cout << "Launching Terminal!" << std::endl;
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
    preboot();

    //Just clear the screen if we are on a Mac. This is my dev PC, and whenever I do a terminal build,
    //there's a ton of crap on the screen!
#ifdef __MACH__
    std::system("clear");
#endif


#ifdef FORK
    pid_t p = fork();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    if(DEBUG)
    {
        std::cout << "PID: " << p << std::endl;
    }
    if(p < 0)
    {
        if(DEBUG)
        {
            std::cout << "Failed to fork! Will run normally!" << std::endl;
        }
    }
    else
    {
        if(p == 0)
        {
            if(DEBUG)
            {
                std::cout << "Child has been spawned!" << std::endl;
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            if(DEBUG)
            {
                std::cout << "Parent will now exit!" << std::endl;
            }
            return 0;
        }
    }
#endif

    boot();
}
