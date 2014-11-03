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
        while(!locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT))) {}
        data[sess].push_back(queue);
        locky_thingy.unlock();
    }
    return;
}

std::string SessionHost::getQueue(std::string sess, unsigned long index)
{
    while(!locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT))) {}
    if(index > data[sess].size())
    {
        locky_thingy.unlock();
        return "";
    }
    else
    {
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

std::string SessionHost::getQueue(std::string sess, unsigned long indl, bool x)
{
    while(!locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT))) {}
    if(indl > data[sess].size())
    {
        locky_thingy.unlock();
        return "";
    }
    else
    {
        std::string x = data[sess][indl];
        locky_thingy.unlock();
        return x;
    }
}

long SessionHost::getQueueDepth(std::string sess)
{
    while(!locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT))) {}
    long i = data[sess].size();
    locky_thingy.unlock();
    return i;
}

void SessionHost::cron()
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        if(DEBUG)
        {
            std::cout << "Cron has started!" << std::endl;
        }

        while(!locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT)))
        {}

        int timethrough = 0;
        std::vector<std::string> del;
        for(std::map<std::string, long>::iterator ite = timestamp.begin(); ite != timestamp.end(); ++ite)
        {
            timethrough++;
            std::cout << "Time through: " << timethrough << std::endl;
            std::string curkey = ite->first;
            long curval = ite->second;
            std::cout << "Key: " << curkey << std::endl;
            if(DEBUG)
            {
                std::cout << "Checking " << curkey << " with old ts of " << curval << std::endl;
            }

            u_int64_t curtm = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch()).count();
            if(DEBUG)
            {
                std::cout << "Current time: " << curtm << std::endl;
            }
            if(curtm - curval > SESSION_TIMEOUT)
            {
                if(DEBUG)
                {
                    std::cout << "Deleted session handle: [" << curkey << "]" << std::endl;
                }
                del.push_back(curkey);
            }
            else
            {
                if(DEBUG)
                {
                    std::cout << "Kept back session handle: [" << curkey << "]" << std::endl;
                }
            }

            for(unsigned int i = 0; i < del.size(); i++)
            {
                timestamp.erase(del[i]);
                data.erase(del[i]);
                std::cout << "Erasing: " << del[i] << std::endl;
            }
        }

        locky_thingy.unlock();
    }
}

/*void SessionHost::cron()
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(20));
        if(!DEBUG)
        {
            std::cout << "Cron Running!" << std::endl;
        }
        while(!locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT))) {}
        for(auto i = timestamp.begin(); i != timestamp.end(); ++i)
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
                if(data.find(i->first) != data.end())
                {
                    data.erase(i->first);
                }
                else
                {
                    std::cout << i->first << " Was not found in data!" << std::endl;
                }
                if(timestamp.find(i->first) != timestamp.end())
                {
                    timestamp.erase(i->first);
                }
                else
                {
                    std::cout << i->first << " Was not found in timestamp!" << std::endl;
                }

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
}*/

void SessionHost::keepAlive(std::string sess)
{
    while(!locky_thingy.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT))) {}
    timestamp[sess] = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch()).count();
    if(DEBUG)
    {
        std::cout << "Sesson: [" << sess << "] reset => " << timestamp[sess] << std::endl;
    }
    locky_thingy.unlock();
    return;
}

SessionHost::SessionHost()
{
    if(DEBUG)
    {
        std::cout << "Constructed SessionHost! Launched cron!" << std::endl;
    }
    cron_thread = std::thread(&SessionHost::cron, this);
}
