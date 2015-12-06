/*
 * TcpServer.cpp
 *
 *  Created on: Nov 10, 2015
 *      Author: yash
 */

#include "TcpServer.h"
#include "ReturnStatusCode.h"

#include <string.h>
#include <stddef.h>
#include <new>
#include <sstream>
#include <thread>

#ifndef _WIN32

#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

#else
//Link the WinSock library
#pragma comment(lib, "Ws2_32.lib")
#include <Windows.h>

//Important for WinSock initialization
static WSADATA WsaData;
static WORD WsaVersionRequested = NULL;
static int Wsaerr = 0;
static bool WsaInitialized = false;

#endif

//Initialize the Server object and it's internal members
daf::TcpServer::Server::Server() :
  FileDescriptor(-1),
  AddressStructure((void*) new struct sockaddr_in),
  ListeningPort(0),
  IsRunning(false),
  TimeoutUsec(0),
  TimeoutSec(0),
  TimeoutStructure(NULL),
  nConnectedClients(0),
  nMaxConnectedClients(0),
  TcpConnectionQueue(3)
{
  //Initialize WinSock (Windows only)
#ifdef _WIN32
  if(!WsaInitialized)
  {
    WsaVersionRequested = MAKEWORD(2, 2);
    Wsaerr = WSAStartup(WsaVersionRequested, &WsaData);

    if(Wsaerr != 0)
    {
      fprintf(stderr, "Unable to find WinSock! Fatal failure.\n");
      exit(EXIT_FAILURE);
    }
  }
#endif
  //Zero out the address structure memory
  if(AddressStructure != NULL)
  {
    memset(AddressStructure, 0, sizeof(struct sockaddr_in));
  }
  else
  {
    throw DafException("Unable to allocate memory for the Socket server address strucure!", -1);
  }
}

daf::TcpServer::Server::~Server()
{
  //Shut down the socket and then close it
#ifndef _WIN32
  shutdown(FileDescriptor, SHUT_RDWR);
  close(FileDescriptor);
#else
  shutdown(FileDescriptor, 1);
  closesocket(FileDescriptor);
#endif

  //Delete address structure
  if(AddressStructure != NULL)
  {
    delete ((struct sockaddr_in*) AddressStructure);
  }
}

daf::TcpServer::Server& daf::TcpServer::Server::startServer()
{
  //Check to see if the server is already running
  if(IsRunning)
    throw DafException("Server is already running", -1);

  //Check to see if the address structure was already allocated
  if(AddressStructure == NULL)
    throw DafException("Address Structure was not allocated! Will not continue", 0);

  //Flag that server is active
  IsRunning = true;

  //Create the socket and check it's validity
  FileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(FileDescriptor < 0)
    throw DafException("Unable to create socket", FileDescriptor);

  //Populate address structure fields
  ((struct sockaddr_in*) AddressStructure)->sin_family = AF_INET;
  ((struct sockaddr_in*) AddressStructure)->sin_addr.s_addr = INADDR_ANY;
  ((struct sockaddr_in*) AddressStructure)->sin_port = htons(ListeningPort);

  //We want to set reuseaddr to help make debugging and using this server less annoying
  int reuseaddr = 1;
#ifndef _WIN32
  setsockopt(FileDescriptor, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int));
#else
  setsockopt(FileDescriptor, SOL_SOCKET, SO_REUSEADDR, (char*) &reuseaddr, sizeof(int));
#endif

  //Bind the port to the address structure. This will assign it's port number
  if(bind(FileDescriptor, (struct sockaddr*) AddressStructure, sizeof(struct sockaddr_in)) < 0)
    throw DafException("Unable to bind to socket address structure", FileDescriptor);

  //Launch the listener. Sadly, this can't be in another thread because the server object goes out of scope
  //and is discarded.
  this->listener();

  //Return a reference to this object so functions can be put one after another
  return (*this);
}

//This function listens for new connections, accepts them and launches them in a separate thread
void daf::TcpServer::Server::listener()
{
  //Begin listening for incoming connection requests.
  //We will allow daf::TcpServer::Server::TcpConnectionQueue number of waiting connections
  listen(FileDescriptor, TcpConnectionQueue);

  int sockaddr_in_size = sizeof(struct sockaddr_in);

  while(true)
  {
    //This points to the connection object which serves the client
    daf::TcpServer::Connection* connection = NULL;

    //Allocate memory or wait 10 msec and try again.
    try
    {
      connection = new daf::TcpServer::Connection;
    }
    catch(std::bad_alloc& e)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    //Check to make sure we don't exceed the number of parallel connections.
    //If we do, poll continuously until we find one.
    while(true)
    {
      //Number of max clients, and number of connected clients
      size_t nmc = 0;
      size_t nc = 0;

      nMaxConnectedClientsMutex.lock();
      nmc = nMaxConnectedClients;
      nMaxConnectedClientsMutex.unlock();

      //If max clients is unlimited
      if(nmc == 0)
        break;

      nConnectedClientsMutex.lock();
      nc = nConnectedClients;
      nConnectedClientsMutex.unlock();

      //Check if we can allocate a session
      if(nc <= nmc)
        break;
    }

    //To make code cleaner
#ifdef _WIN32
    typedef int socklen_t;
#endif

    //Accept the new connection.
    connection->FileDescriptor = accept(
      FileDescriptor,
      (struct sockaddr*) connection->AddressStructure,
      (socklen_t*) &sockaddr_in_size
    );

    //Check to make sure the accept was successful, otherwise, discard everything and try again.
    if(connection->FileDescriptor < 0)
    {
      delete connection;
      continue;
    }

    //Set the timeout structure
    if(TimeoutStructure == NULL)
      TimeoutStructure = ((void*) new struct timeval);

    ((struct timeval*) TimeoutStructure)->tv_sec = TimeoutSec;
    ((struct timeval*) TimeoutStructure)->tv_usec = TimeoutUsec;

    //Set the socket read/write timeout
#ifndef _WIN32
    if(setsockopt(connection->FileDescriptor, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*) &TimeoutStructure, sizeof(struct timeval)))
#else
    if(setsockopt(connection->FileDescriptor, SOL_SOCKET, SO_RCVTIMEO, (char*) &TimeoutStructure, sizeof(struct timeval)))
#endif
    {
      delete connection;
      continue;
    }

    //increment the connected clients' counter
    nConnectedClientsMutex.lock();
    nConnectedClients++;
    nConnectedClientsMutex.unlock();

    //Assign the listening server to this. It allows access to the server's internal variables
    //Mainly used for the connected user counter.
    connection->ListeningServer = this;

    //Create the new thread. The listenerProxy will handle everything else. If fail, delete everything
    //and start fresh
    try
    {
      std::thread(&daf::TcpServer::Server::listenerProxy, this, (void*) connection).detach();
    }
    catch(std::exception& e)
    {
      delete connection;
      continue;
    }
  }
}

void daf::TcpServer::Server::listenerProxy(void* connection)
{
  //Useless really, but for clarity
  daf::TcpServer::Connection* conn = ((daf::TcpServer::Connection*) connection);

  //Encapsulate everything in a try to prevent an exception from crashing the thread [FATAL]
  try
  {
    //Run the worker function and do GC
    worker(conn);
    delete conn;
    return;
  }
  catch(std::exception& e)
  {
    //Do GC
    delete conn;
    return;
  }
}

//This function allows you to interact with the server
void daf::TcpServer::Server::worker(daf::TcpServer::Connection* connection)
{
  connection->write("Hello World!\n");
}

//This function initializes the connection object so it's ready to use
daf::TcpServer::Connection::Connection() :
  FileDescriptor(0),
  AddressStructure((void*) new struct sockaddr_in),
  ListeningServer(NULL)
{
  if(AddressStructure != NULL)
    memset(AddressStructure, 0, sizeof(struct sockaddr_in));
}

daf::TcpServer::Connection::~Connection()
{
  //Decrement the connected user counter
  this->ListeningServer->nConnectedClientsMutex.lock();
  this->ListeningServer->nConnectedClients--;
  this->ListeningServer->nConnectedClientsMutex.unlock();

  //Shutdown and close the socket
#ifndef _WIN32
  shutdown(FileDescriptor, SHUT_RDWR);
  close(FileDescriptor);
#else
  shutdown(FileDescriptor, 1);
  closesocket(FileDescriptor);
#endif

  //GC
  if(AddressStructure != NULL)
  {
    delete ((struct sockaddr_in*) AddressStructure);
  }
}

//This function reads until char end is received
std::string daf::TcpServer::Connection::readline(char end)
{
  std::stringstream str;
  char ch;

  while((ch = this->read()) != end)
    str << ch;

  std::string string = str.str();
  return string;
}

//Read a line, ending in `char end`
int daf::TcpServer::Connection::readline(char* buffer, size_t bufsz, char end)
{
  char* chl = buffer;
  size_t spos = 0;

  while(spos < bufsz)
  {
    spos++;
    (*chl) = this->read();
    if((char)(*chl) == (char) end)
    {
      *chl = '\0';
      break;
    }
    chl++;
  }

  return (int) spos;
}

//Read a byte
char daf::TcpServer::Connection::read()
{
  char ch;
  ssize_t ret;
#ifdef _WIN32
  if((ret = recv(FileDescriptor, &ch, sizeof(char), 0)) <= 0)
#else
  if((ret = recv(FileDescriptor, &ch, sizeof(char), MSG_NOSIGNAL)) <= 0)
#endif
  {
    if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINPROGRESS)
      throw DafException("Socket read timed out", errno);
    throw DafException("Unable to read from pipe", errno);
  }

  return ch;
}

//Write a byte
ssize_t daf::TcpServer::Connection::write(char ch)
{
  ssize_t ret;
#ifdef _WIN32
  if((ret = send(FileDescriptor, &ch, sizeof(char), 0)) <= 0)
#else
  if((ret = send(FileDescriptor, &ch, sizeof(char), MSG_NOSIGNAL)) <= 0)
#endif
  {
    throw DafException("Unable to write to socket", errno);
  }

  return ret;
}

//Write a string
ssize_t daf::TcpServer::Connection::write(std::string str)
{
  ssize_t ret;
#ifdef _WIN32
  if((ret = send(FileDescriptor, str.c_str(), str.size(), 0)) <= 0)
#else
  if((ret = send(FileDescriptor, str.c_str(), str.size(), MSG_NOSIGNAL)) <= 0)
#endif
  {
    throw DafException("Unable to write to socket", errno);
  }

  return ret;
}

//Write a data pointer
ssize_t daf::TcpServer::Connection::write(void* data, size_t len)
{
  ssize_t ret;
#ifdef _WIN32
  if((ret = send(FileDescriptor, (char*) data, len, 0)) <= 0)
#else
  if((ret = send(FileDescriptor, (char*) data, len, MSG_NOSIGNAL)) <= 0)
#endif
  {
    throw DafException("Unable to write to socket", errno);
  }

  return ret;
}
