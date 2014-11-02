#include "../include/Session.hxx"

//To compare two strings to each other. Note that this function is private to only this source file
inline bool operator== (std::string& a, std::string& b)
{
    return !strcmp(a.c_str(), b.c_str());
}

//To stringstream data for me. I'm too lazy to write three lines of code. Why not one? :)
template <typename T> void cNumber(std::string& in, T& out)
{
    std::stringstream str;
    str << in;
    str >> out;
    return;
}

void SessionHost::dequeue(std::string sess, std::string queue)
{
    keepAlive(sess);
    if(queue.size() == 0)
    {
        return;
    }
    else
    {
        locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT));
        data[sess].push_back(queue);
        locky_thingy.unlock();
    }
}

std::string SessionHost::getQueue(std::string sess, long index)
{
    locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT));
    if(index > data[sess].size())
    {
        locky_thingy.unlock();
        return "";
    }
    else
    {
        locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT));
        std::stringstream s;
        if(DEBUG)
        {
            std::cout << "Queues: " << data[sess].size() << std::endl;
        }

        for(unsigned long i = index; i < data[sess].size(); i++)
        {
            s << data[sess][i];
        }
        locky_thingy.unlock();
        return s.str();
    }
}

std::string SessionHost::getQueue(std::string sess, long indl, bool x)
{
    locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT));
    if(indl > data[sess].size())
    {
        locky_thingy.unlock();
        return "";
    }
    else
    {
        locky_thingy.unlock();
        return data[sess][indl];
    }
}

long SessionHost::getQueueDepth(std::string sess)
{
    locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT));
    return data[sess].size();
    locky_thingy.unlock();
}

void SessionHost::cron()
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(20));
        if(DEBUG)
        {
            std::cout << "Cron Running!" << std::endl;
        }
        while(!locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT)));
        for(auto i = data.begin(); i != data.end(); ++i)
        {
            if(DEBUG)
            {
                std::cout << "Checking " << i->first << " with timestamp: " << timestamp[i->first] << std::endl;
            }
            unsigned long x = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch()).count();
            if(DEBUG)
            {
                std::cout << "Current time: " << x << std::endl;
            }
            unsigned long tdiff = x - timestamp[i->first];
            if(tdiff > SESSION_TIMEOUT)
            {
                timestamp.erase(i->first);
                data.erase(i->first);
                if(DEBUG)
                {
                    std::cout << "Killed Session [" << i->first << "]!" << std::endl;
                }
            }
            else
            {
                if(DEBUG)
                {
                    std::cout << "Skipped Session [" << i->first << "]!" << std::endl;
                }
            }
        }
        locky_thingy.unlock();
    }
}

void SessionHost::keepAlive(std::string sess)
{
    locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT));
    timestamp[sess] = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch()).count();
    locky_thingy.unlock();
}

SessionHost::SessionHost()
{
    cron_thread = std::thread(&SessionHost::cron, this);
}
