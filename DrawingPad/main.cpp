#include <iostream>
#include <thread>
#include <string>
#include "include/server.hxx"
#include "include/Terminal.hxx"
#include "include/definitions.hxx"

using namespace std;

int main()
{
    //Just clear the screen if we are on a Mac. This is my dev PC, and whenever I do a terminal build,
    //there's a ton of crap on the screen!
#ifdef __MACH__
    std::system("clear");
#endif

    paint_server serv;
    serv.set_listening_port(1234);
    std::cout << "PaintServer Started...!" << std::endl;
    serv.start_async();
    std::thread terminal_thread(&terminal);
    terminal_thread.detach();
    while(true) std::this_thread::sleep_for(std::chrono::seconds(100));
    return 0;
}
