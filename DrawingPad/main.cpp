#include <iostream>
#include <thread>
#include "include/server.hxx"
#include "include/Terminal.hxx"

using namespace std;

void hello()
{
    std::cout << "Hello World!" << std::endl;
}

int main()
{
#ifdef __MACH__
    std::system("clear");
#endif
    //std::thread x(hello);
    //x.detach();
    std::cout << std::endl;
    paint_server serv;
    serv.set_listening_port(1234);
    serv.start_async();
    std::thread terminal_thread(&terminal);
    terminal_thread.detach();
    while(true) std::this_thread::sleep_for(std::chrono::seconds(100));
    return 0;
}
