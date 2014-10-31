#ifndef RAMFS_HXX
#define RAMFS_HXX
#include <unordered_map>
#include <thread>
#include <mutex>
#include <fstream>
#include <vector>
#include "definitions.hxx"
#include <sstream>
#include <iostream>

namespace ramfs
{
    bool stat_file(std::string file_location);
    std::string read_file(std::string file_location);
    void write_file(std::string file_location, std::string file_data);
    void write_file_async(std::string file_location, std::string file_data);

    class ramfs
    {
    private:
        std::unordered_map<std::string, std::string> filesystem;
        std::timed_mutex mtx;

        std::string get(std::string file_location);
        void put(std::string file_location, std::string file_data);
        bool stat_hid(std::string file_location);
    public:
        bool cache_file(std::string file_location);
        void cache_list(std::vector<std::string> file_locations);
        bool stat(std::string file_location);
        std::string get_file(std::string file_location);
        std::string get_file_autocache(std::string file_location);
        void put_file(std::string file_location, std::string file_data);
        void put_file_async(std::string file_location, std::string file_data);
        void put_file_async_on_disk(std::string file_location, std::string file_data);
        void put_file_async_on_disk_async(std::string file_locationm, std::string file_data);
    };

    static ramfs filesystem;
}
#endif // RAMFS_HXX
