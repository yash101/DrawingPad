/*
 * Server.h
 *
 *  Created on: Nov 11, 2015
 *      Author: yash
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_
#include "TcpServer.h"
#include "ReturnStatusCode.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#ifdef _WIN32
typedef long long ssize_t;
#endif

//Default namespace
namespace daf
{
  //Stores HTTP functions, enumerations and typical classes
  namespace Http
  {
    //What type of request are we receiving
    enum RequestType
    {
      GET,
      POST,
      HEAD,
      PUT,
      DEL,
      CONNECT,
      OPTIONS,
      TRACE
    };

    //What HTTP protocol version are we using
    enum Protocol
    {
      HTTP10,
      HTTP11
    };

    //Where is a piece of data being stored
    enum DataSource
    {
      FILE,
      STRING,
      CACHE
    };

    //Stores data and what type of data is being stored
    class Data
    {
    public:
      ::FILE* file;
      DataSource DataType;
      std::string string;
      std::string cache_key;

      inline Data() :
        file(NULL),
        DataType(STRING)
      {
      }

      inline virtual ~Data()
      {
        if(DataType == FILE && file != NULL)
          fclose(file);
      }
    };

    //Encodes and decodes URLs
    std::string encodeURI(std::string in);
    std::string decodeURI(std::string in);
    void iencodeURI(std::string& in);
    void idecodeURI(std::string& in);

    //Gets timestamps and converts a status code to a status string
    std::string timestamp();
    std::string statusString(short int code);
  }

  //Stores HTTP server classes
  namespace HttpServer
  {
    class Server;           //Main server
    class ReturnStatusCode; //A typical ReturnStatusCode (see ReturnStatusCode.h), but has an extra field for HTTP debugging
    class Exception;        //Throwable exception around ReturnStatusCode
    class Session;          //Holds all the information for a request and a response
    class Socket;           //Allows interaction with an HTTP socket

    class Server : public daf::TcpServer::Server
    {
      friend class Session;
      friend class Socket;
    protected:

      void worker(daf::TcpServer::Connection* connection);

      void bad_request(daf::HttpServer::Session* session, daf::HttpServer::Exception& e);

      void process_request(daf::HttpServer::Session* session);
        void parse_get_queries(daf::HttpServer::Session* session, std::vector<std::string>& parts);
        void parse_post_queries(daf::HttpServer::Session* session);
      void check_request(daf::HttpServer::Session* session);
      void prepare_session(daf::HttpServer::Session* session);

      virtual void request_handler(daf::HttpServer::Session& session);
      virtual void websocket_handler(daf::HttpServer::Socket& socket);

      void check_session_response(daf::HttpServer::Session* session);
      void send_response(daf::HttpServer::Session* session);

    public:
      Server();
      virtual ~Server();
    };

    class Session
    {
      friend class Server;
      friend class Socket;

    protected:

    public:

      daf::TcpServer::Connection* TcpConnection;
      daf::Http::RequestType RequestType;
      daf::Http::Protocol Protocol;

      std::string Path;
      std::string CompletePath;

      daf::Http::Data Response;
      short int StatusCode;

      std::map<std::string, std::string> Get;
      std::map<std::string, daf::Http::Data> Post;
      std::map<std::string, std::string> IncomingHeaders;
      std::map<std::string, std::string> Headers;
      std::map<std::string, std::string> IncomingCookies;
      std::map<std::string, std::string> Cookies;
    };

    class Socket
    {
      friend class Server;
      friend class Session;

    protected:
    public:
    };

    class ReturnStatusCode
    {
    private:
      const short int HttpStatusCode;
      const char* Message;
      const int Code;
      const char* Source;
      const unsigned long long LineNumber;
    public:
      inline ReturnStatusCode() :
        HttpStatusCode(500),
        Message("An error occurred during the processing of the transaction"),
        Code(0),
        Source(__FILE__),
        LineNumber(__LINE__)
      {
      }

      inline ReturnStatusCode(
        const char* message,
        const int code,
        const short int httpstatus,
        const char* source,
        const unsigned long long line_num
      ) :
        HttpStatusCode(httpstatus),
        Message(message),
        Code(code),
        Source(source),
        LineNumber(line_num)
      {
      }

      inline virtual ~ReturnStatusCode()
      {
      }

      inline short int getHttpStatusCode()
      {
        return HttpStatusCode;
      }

      inline const char* getMessage()
      {
        return Message;
      }

      inline int getCode()
      {
        return Code;
      }

      inline const char* getSourceLocation()
      {
        return Source;
      }

      inline unsigned long long getLineNumber()
      {
        return LineNumber;
      }
    };

    class Exception : public std::exception
    {
    private:
      const ReturnStatusCode rsc;
      const char* msg;
      int backtrace_size;
      char** backtrace_strings;
      void generateStacktrace();
    public:
      inline Exception() :
        msg("An exception was thrown"),
        backtrace_size(0),
        backtrace_strings(NULL)
      {
        this->generateStacktrace();
      }

      inline Exception(
        const char* message,
        const int code,
        const short int httpstatus,
        const char* source,
        const unsigned long long line_num
      ) :
        rsc(message, code, httpstatus, source, line_num),
        msg(message),
        backtrace_size(0),
        backtrace_strings(NULL)
      {
        this->generateStacktrace();
      }

      virtual ~Exception();

      inline virtual const char* what() const throw()
      {
        return msg;
      }

      inline ReturnStatusCode getStatusCode()
      {
        return rsc;
      }

      std::string getStacktrace();
    };
  }
}

#endif /* HTTPSERVER_H_ */
