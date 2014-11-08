#include "../include/RAMFS.hxx"

std::string ramfs::read_file(std::string file_location)
{
    std::ifstream fin(file_location.c_str());
    if(fin.is_open())
    {
        std::stringstream x;
        x << fin.rdbuf();
        fin.close();
        return x.str();
    }
    return "";
}

bool ramfs::stat_file(std::string file_location)
{
    std::ifstream fin(file_location.c_str());
    if(fin.is_open())
    {
        fin.close();
        return true;
    }
    else
    {
        return false;
    }
}

void ramfs::write_file(std::string file_location, std::string file_data)
{
    std::ofstream fout(file_location.c_str());
    if(fout.is_open())
    {
        fout << file_data;
        fout.close();
    }
    return;
}

void ramfs::write_file_async(std::string file_location, std::string file_data)
{
    std::thread x(write_file, file_location, file_data);
    x.detach();
    return;
}

void ramfs::ramfs::put(std::string file_location, std::string file_data)
{
#ifndef DISABLE_FILECACHE
    mtx.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT));
    filesystem[file_location] = file_data;
    mtx.unlock();
#endif
    return;
}

std::string ramfs::ramfs::get(std::string file_location)
{
#ifndef DISABLE_FILECACHE
    mtx.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT));
    std::string x = filesystem[file_location];
    mtx.unlock();
#else
    return read_file(file_location);
#endif
#ifndef DISABLE_FILECACHE
    return x;
#endif
}

bool ramfs::ramfs::cache_file(std::string file_location)
{
    if(stat_file(file_location))
    {
        put(file_location, read_file(file_location));
        return true;
    }
    else
    {
        return false;
    }
}

void ramfs::ramfs::sync()
{
    for(auto it = filesystem.begin(); it != filesystem.end(); it++)
    {
        if(DEBUG)
        {
            std::cout << "Recaching file..." << it->first;
        }
        if(stat_file(it->first))
        {
            std::cout << "Success!" << std::endl;
            put(it->first, read_file(it->first));
        }
        else
        {
            std::cout << "Fail!" << std::endl;
        }
    }
}

void ramfs::ramfs::cache_list(std::vector<std::string> file_locations)
{
    for(unsigned int i = 0; i < file_locations.size(); i++)
    {
        if(DEBUG)
        {
            std::cout << "Caching file: " << file_locations[i] << "...";
        }
        if(cache_file(file_locations[i]))
        {
            std::cout << "Success" << std::endl;
        }
        else
        {
            if(DEBUG)
            {
                std::cout << "Failed!" << std::endl;
                std::cout << "Failed because unable to stat " << file_locations[i] << std::endl;
            }
        }
    }
}

inline bool ramfs::ramfs::stat_hid(std::string file_location)
{
    mtx.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT));
    std::unordered_map<std::string, std::string>::const_iterator iterator = filesystem.find(file_location);
    bool y = iterator != filesystem.end();
    mtx.unlock();
    return y;
}

bool ramfs::ramfs::stat(std::string file_location)
{
    return stat_hid(file_location);
}

std::string ramfs::ramfs::get_file(std::string file_location)
{
    return get(file_location);
}

std::string ramfs::ramfs::get_file_autocache(std::string file_location)
{
    if(stat_hid(file_location))
    {
        return get(file_location);
    }
    else
    {
        put(file_location, read_file(file_location));
        return get(file_location);
    }
}

void ramfs::ramfs::put_file(std::string file_location, std::string file_data)
{
    put(file_location, file_data);
    return;
}

void ramfs::ramfs::put_file_async(std::string file_location, std::string file_data)
{
    std::thread x(&ramfs::put_file, this, file_location, file_data);
    x.detach();
    return;
}

void ramfs::ramfs::put_file_async_on_disk(std::string file_location, std::string file_data)
{
    put_file_async(file_location, file_data);
    write_file_async(file_location, file_data);
    return;
}
