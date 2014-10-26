#include <iostream>
#include <thread>

using namespace std;

int main()
{
    std::thread x([](){
        std::cout << "Hello World!" << std::endl;
        while(true) {}
    });
    x.detach();
    return 0;
}
