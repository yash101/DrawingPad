#include "../include/Session.hxx"
//To compare two strings to each other. Note that this function is private to only this source file

inline bool operator== (std::string& a, std::string& b)
{
    return !strcmp(a.c_str(), b.c_str());
}

template <typename T> void cNumber(std::string& in, T& out)
{
    std::stringstream str;
    str << in;
    str >> out;
    return;
}

std::string Session::getqueue()
{
    return queue_out.str();
}

void Session::dequeue_new(std::string queue)
{
    if(queue.size() == 0)
    {
        return;
    }
    queues.push_back(queue);

    std::string buf;
    std::stringstream q;
    q << queue;

    while(std::getline(q, buf, ' '))
    {
        if(buf == "begpath")
        {
            std::cout << "Beginning Path; ";
            std::getline(q, buf, ' ');
            std::cout << "Setting Color: " << buf << "; ";
            std::getline(q, buf, ' ');
            std::cout << "Canvas Width: " << buf << "; ";
            std::getline(q, buf, ' ');
            std::cout << "Canvas Height: " << buf << "; ";
            std::getline(q, buf, ' ');
            std::cout << "Cursor X" << buf << "; ";
            std::getline(q, buf, ' ');
            std::cout << "Cursor Y" << buf << "; ";
        }

        if(buf == "point")
        {
            std::string a,b;
            std::getline(q, a, ' ');
            std::getline(q, b, ' ');
            std::cout << "Point<" << a << "," << b << ">" << " ;";
        }

        if(buf == "endpath")
        {
            std::cout << "Path Complete!" << std::endl;
        }
    }
}

std::string Session::getQueue(long index)
{
    if(index > queues.size())
    {
        if(DEBUG)
        {
            std::cout << "Uh Oh! Index is high! :(" << std::endl;
            return "Index Invalid!";
        }
    }
    else
    {
        std::stringstream str;
        std::cout << "Building Queue!" << std::endl << "Queue Depth: " << queues.size() << std::endl;
        for(unsigned int i = index; i < queues.size(); i++)
        {
            std::cout << "QUEUE: " << i << " : " << queues[i] << std::endl;
            str << queues[i];
        }
        return str.str();
    }
}

long Session::getQueueDepth()
{
    return queues.size();
}
