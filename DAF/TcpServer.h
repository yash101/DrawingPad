/*
 * TcpServer.h
 *
 *  Created on: Nov 10, 2015
 *      Author: yash
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#ifdef _WIN32
typedef long long ssize_t;
#endif

#include <string>
#include <mutex>

#include "ReturnStatusCode.h"

namespace daf
{
  namespace TcpServer
  {
    class Server;
    class Connection;

    class Server
    {
      friend class Connection;
    protected:
      int FileDescriptor;
      void* AddressStructure;
      int ListeningPort;
      bool IsRunning;
      int TimeoutUsec;
      int TimeoutSec;
      void* TimeoutStructure;

      size_t nConnectedClients;
      size_t nMaxConnectedClients;

      int TcpConnectionQueue;

      std::mutex nConnectedClientsMutex;
      std::mutex nMaxConnectedClientsMutex;

      void listener();
      void listenerProxy(void* connection);

      virtual void worker(Connection* connection);


    public:
      Server();
      virtual ~Server();

      Server& startServer();


      inline Server& setPort(int port)
      {
        ListeningPort = port;
        return (*this);
      }

      inline Server& setSocketTimeout(int usec, int sec)
      {
        TimeoutUsec = usec;
        TimeoutSec = sec;
        return (*this);
      }

      inline Server& setMaxConnectedClients(size_t max)
      {
        nMaxConnectedClients = max;
        return (*this);
      }

      inline Server& setTcpQueueSize(int n)
      {
        TcpConnectionQueue = n;
        return (*this);
      }

      inline int getPort()
      {
        return ListeningPort;
      }

      inline int isServerRunning()
      {
        return IsRunning;
      }

      inline int getSocketTimeoutUsec()
      {
        return TimeoutUsec;
      }

      inline int getSocketTimeoutSec()
      {
        return TimeoutSec;
      }

      inline int getTcpQueueSize()
      {
        return TcpConnectionQueue;
      }

      inline size_t getNumberOfConnectedClients()
      {
        size_t n = 0;
        nConnectedClientsMutex.lock();
        n = nConnectedClients;
        nConnectedClientsMutex.unlock();
        return n;
      }
    };

    class Connection
    {
      friend class Server;
    private:
      int FileDescriptor;
      void* AddressStructure;
      Server* ListeningServer;
    public:
      Connection();
      virtual ~Connection();

      std::string readline(char end);
      int readline(char* buffer, size_t bufsz, char end);

      char read();
      std::string read(size_t maxlen);
      int read(void* buffer, size_t bufsz);

      ssize_t write(char ch);
      ssize_t write(std::string data);
      ssize_t write(void* data, size_t bufsz);

      inline int getFileDescriptor()
      {
        return FileDescriptor;
      }
    };
  }
}

#endif /* TCPSERVER_H_ */
