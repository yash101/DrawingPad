/*
 * Server.cpp
 *
 *  Created on: Nov 11, 2015
 *      Author: yash
 */

#include "HttpServer.h"
#include "stringproc.h"
#include <string>
#include <stdlib.h>

#ifndef _WIN32
#include <sys/sendfile.h>
#include <execinfo.h>
#endif

//To make it easier to throw an exception with debug info :)
#define HttpException(msg, code, httpstat) (daf::HttpServer::Exception(msg, code, httpstat, __FILE__, __LINE__))
#define MAX_HTTP_LINE_LENGTH (16384)
#define String std::string
#define Vector std::vector
#define ToString std::to_string

//Typcial initializers and destructors
daf::HttpServer::Server::Server()
{
}

daf::HttpServer::Server::~Server()
{
}

//This function is called upon an accepted incoming request
void daf::HttpServer::Server::worker(daf::TcpServer::Connection* connection)
{
  //Create the session object
  daf::HttpServer::Session session;
  session.TcpConnection = connection;

  //Process the request completely
  try
  {
    this->process_request(&session);
    this->check_request(&session);
    this->prepare_session(&session);
    this->request_handler(session);
    this->check_session_response(&session);
    this->send_response(&session);
  }
  //Catch all errors. We cannot let these escape. A crashed thread means a catastrophic program crash!
  catch(daf::HttpServer::Exception& e)
  {
    bad_request(&session, e);
  }
  catch(std::exception& e)
  {
  }
}

//Sends back an error report with a stacktrace
void daf::HttpServer::Server::bad_request(daf::HttpServer::Session* session, daf::HttpServer::Exception& e)
{
  if(e.getStatusCode().getHttpStatusCode() < 0) return;
  std::stringstream write;
  write << "<!DOCTYPE html>";
#define o write << std::endl <<
  o "<html>";
    o "<head>";
      o "<title>Error " << e.getStatusCode().getHttpStatusCode() << ": " << e.getStatusCode().getMessage() << "</title>";
    o "</head>";
    o "<body>";
      o "<h1><center>Error " << e.getStatusCode().getHttpStatusCode() << ": " << e.getStatusCode().getMessage() << "</center></h1>";
      o "<p>The information you sent us resulted in an error during processing!</p>";
      o "<h2>Error Information and Traceback</h2>";
      o "<div style=\"background-color: #C6C6C6; margin: 32px; padding: 32px;\">";
        o "<h3><center>Information</center></h3>";
        o "<hr />";
        o "<pre>";
          o "Message: " << e.getStatusCode().getMessage();
          o "Error Code: " << e.getStatusCode().getCode();
          o "Status Code: " << e.getStatusCode().getHttpStatusCode();
          o "Error in file: " << e.getStatusCode().getSourceLocation();
          o "Error line number: " << e.getStatusCode().getLineNumber();
          o "Processed Path: " << session->Path;
          o "Path: " << session->CompletePath;
        o "</pre>";
      o "</div>";
      o "<div style=\"background-color: #C6C6C6; margin: 32px; padding: 32px;\">";
        o "<h3><center>Stacktrace</center></h3>";
        o "<hr />";
        o "<pre>";
          o e.getStacktrace();
        o "</pre>";
      o "</div>";
      o "<div style=\"height: 30px;\"></div>";
      o "<p style=\"background-color: #C6C6C6;\">Powered by <a href=\"https://github.com/yash101/ApplicationServer/\">ApplicationServer</a>!</p>";
    o "</body>";
  o "</html>";
#undef o

  std::stringstream send;
  send << "HTTP/1.0 " << e.getStatusCode().getHttpStatusCode() << " " << daf::Http::statusString(e.getStatusCode().getHttpStatusCode()) << "\r\n";
  send << "Content-Length: " << write.str().size() << "\r\n";
  send << "Content-Type: text/html\r\n";
  send << "Date: " << daf::Http::timestamp() << "\r\n\r\n";

  send << write.str() << "\r\n";

  session->TcpConnection->write(send.str());
}

//Processes a request
void daf::HttpServer::Server::process_request(daf::HttpServer::Session* session)
{
  //Buffer holds preprocessed data
  char flbuf[MAX_HTTP_LINE_LENGTH];
  flbuf[MAX_HTTP_LINE_LENGTH - 1] = '\0';

  //Read the first line
  if(session->TcpConnection->readline(flbuf, MAX_HTTP_LINE_LENGTH, '\n') >= MAX_HTTP_LINE_LENGTH)
  {
    throw HttpException("Bad Request: HTTP Field maximum length reached!", MAX_HTTP_LINE_LENGTH, 400);
  }

  //Process the first line
  String str = flbuf;
  if(str.back() == '\r' || str.back() == '\n') str.pop_back();
  daf::itrim(str);

  //Split the first line by spaces
  Vector<String> parts = daf::split(str, ' ');

  //Return if we didn't get as many parts as we should have
  //Bad request!
  if(parts.size() < 3)
  {
    throw HttpException(
      std::string("400: Bad Request; Could not understand first line!\nReceived \"" + str + "\"!").c_str(),
      (int) parts.size(),
      400
    );
  }

  //Select what type of request we are handling
  //_ To make our code cleaner :)
#define w (parts[0])
#define s session->RequestType =

  if(w == "GET") s daf::Http::GET;
  else if(w == "POST") s daf::Http::POST;
  else if(w == "HEAD") s daf::Http::HEAD;
  else if(w == "PUT") s daf::Http::PUT;
  else if(w == "DEL") s daf::Http::DEL;
  else if(w == "CONNECT") s daf::Http::CONNECT;
  else if(w == "OPTIONS") s daf::Http::OPTIONS;
  else if(w == "TRACE") s daf::Http::TRACE;
  else
  {
    char buffer[16384];
    sprintf(buffer, "501 Unimplemented: \"%s\" has not been implemented or could not be understood!", w.c_str());
    throw HttpException(buffer, -1, 501);
  }

  //Clean up after ourselves (unneccessary)
#undef w
#undef s

  this->parse_get_queries(session, parts);

  //Get rid of the spacing in the protocol revision ID and select it
  daf::itrim(parts[2]);
  if(parts[2] == "HTTP/1.0") session->Protocol = daf::Http::HTTP10;
  else if(parts[2] == "HTTP/1.1") session->Protocol = daf::Http::HTTP11;
  else throw HttpException("400 Bad Request: Unknown protocol", -1, 400);

  //Download and save headers
  while(true)
  {
    //Read a line
    if(session->TcpConnection->readline(flbuf, MAX_HTTP_LINE_LENGTH, '\n') >= MAX_HTTP_LINE_LENGTH)
    {
      throw HttpException("400 Bad Request: Maximum HTTP Field Length Reached!", MAX_HTTP_LINE_LENGTH, 400);
    }

    //Process the line
    str = flbuf;
    if(str.back() == '\r' || str.back() == '\n') str.pop_back();

    daf::itrim(str);

    if(str.size() == 0)
      break;

    ssize_t pos = daf::find(str, ':');
    if(pos <= 0) continue;

    //Break the line
    String key = daf::tolower(daf::trim(str.substr(0, pos)));
    String value = daf::trim(str.substr(pos + 1, str.size()));

    //Parse if cookie
    if(key == "cookie")
    {
      Vector<String> cookies = daf::split(value, ';');
      for(size_t i = 0; i < cookies.size(); i++)
      {
        pos = daf::find(cookies[i], '=');

        key = daf::trim(cookies[i].substr(0, pos));
        value = daf::trim(cookies[i].substr(pos + 1, cookies[i].size()));

        session->IncomingCookies[String(key)] = String(value);
      }
    }
    else
    {
      //Otherwise, save as an incoming header
      session->IncomingHeaders[std::string(key.c_str())] = std::string(value.c_str());
    }
  }

  parse_post_queries(session);
}

//Processes the path. Retrieves URL encoded values
void daf::HttpServer::Server::parse_get_queries(daf::HttpServer::Session* session, Vector<String>& parts)
{
  //Split the path off the first question mark
  session->CompletePath = parts[1];
  ssize_t loc = daf::find(parts[1], '?');
  if(loc < 0)
  {
    session->Path = parts[1];
  }
  else
  {
    //Separate the path from the query
    session->Path = parts[1].substr(0, loc);
    String qstr = parts[1].substr(loc + 1, parts[1].size());
    //Split the query up into it's queries
    Vector<String> queries = daf::split(qstr, '&');
    //Add each query after separating them into key and value
    for(size_t i = 0; i < queries.size(); i++)
    {
      if(queries[i].size() == 0)
        continue;
      ssize_t pos = daf::find(queries[i], '=');
      if(pos == 0) continue;
      if(pos < 0)
        session->Get[daf::Http::decodeURI(queries[i])] = "";
      else
        session->Get[daf::Http::decodeURI(queries[i].substr(0, pos))] = daf::Http::decodeURI(queries[i].substr(pos + 1, queries[i].size()));
    }
  }
}

void daf::HttpServer::Server::parse_post_queries(daf::HttpServer::Session* session)
{
  //Process POST-encoded requests
  if(session->RequestType == daf::Http::POST)
  {
    //Check the encoding of the POST request
    if(daf::tolower(session->IncomingHeaders["content-type"]) == "application/x-www-form-urlencoded")
    {
      //Check the content-length header for the amount of data to wait for
      if(session->IncomingHeaders["content-length"].size() != 0)
      {
        //Retrieve the number of bytes to download
        size_t len = atoll(session->IncomingHeaders["content-length"].c_str());
        daf::Http::DataSource tp = daf::Http::STRING;

        String key;
        String value;

        //Byte counter; Counts how many bytes were downloaded in total
        size_t i = 0;

        //Repeat for each query (until the byte counter reaches Headers->content-length
        while(i < len)
        {
          //Downnload until the equal sign. Equal sign separates the key from the value
          while(i < len && key.back() != '=')
          {
            key += (char) session->TcpConnection->read();
            i++;
          }

          //Delete the equal sign
          key.pop_back();

          //Read until we get an ampersand. '&' separates multiple queries
          while(i < len && value.back() != '&')
          {
            value += (char) session->TcpConnection->read();
            i++;
          }

          //Stores the POST data
          daf::Http::Data post;
          post.DataType = tp;
          post.string = daf::Http::decodeURI(value);

          //Set the POST value in the POST map
          session->Post[daf::Http::decodeURI(key)] = post;
        }
      }
    }
    //All other POST protocols are currently unspported
    else
    {
      throw HttpException("Unable to parse POST request. Multipart post not implemented!", -1, 501);
    }
  }
}

//Checks for the safety of the request. If the protocol is not HTTP 1.0, we should have received a
//HOST field
void daf::HttpServer::Server::check_request(daf::HttpServer::Session* session)
{
  if(session->Protocol != daf::Http::HTTP10)
  {
    if(session->IncomingHeaders["host"].size() == 0)
    {
      throw HttpException("No Host Provided! Routing failed!", 400, 400);
    }
  }
}

//Set the default values in the session
//Mime and status code
void daf::HttpServer::Server::prepare_session(daf::HttpServer::Session* session)
{
  session->Headers["content-type"] = "text/plain";
  session->StatusCode = 200;
}

//Check the response from the request handler
void daf::HttpServer::Server::check_session_response(daf::HttpServer::Session* session)
{
  //Check to make sure we have a content-type field
  if(session->Headers["content-type"].size() == 0)
    session->Headers["content-type"] = "octet/stream";

  //Set the content-length field
  if(session->Response.DataType == daf::Http::FILE)
  {
    if(session->Response.file == NULL || ftell(session->Response.file) < 0)
      throw HttpException("Attempting to send unopened file!", 500, 500);
    fseek(session->Response.file, 0, SEEK_END);
    session->Headers["content-length"] = ToString(ftell(session->Response.file));
    fseek(session->Response.file, 0, SEEK_SET);
  }
  else
  {
    session->Headers["content-length"] = ToString(session->Response.string.size());
  }

  //Send a timestamp
  session->Headers["date"] = daf::Http::timestamp();
}

//Write back the response
void daf::HttpServer::Server::send_response(daf::HttpServer::Session* session)
{
  //Write the first line
  session->TcpConnection->write((session->Protocol == daf::Http::HTTP10) ? "HTTP/1.0 " : "HTTP/1.1 ");
  session->TcpConnection->write(ToString(session->StatusCode));
  session->TcpConnection->write(String(" " + daf::Http::statusString(session->StatusCode) + "\r\n").c_str());

  //Write all headers
  for(std::map<std::string, std::string>::const_iterator it = session->Headers.begin(); it != session->Headers.end(); ++it)
  {
    session->TcpConnection->write(String(it->first + ": " + it->second + "\r\n").c_str());
  }

  //Write all cookies
  for(std::map<std::string, std::string>::const_iterator it = session->Cookies.begin(); it != session->Cookies.end(); ++it)
  {
    session->TcpConnection->write(String("Set-Cookie: " + it->first + "=" + it->second + "\r\n").c_str());
  }

  //Write a newline before sending the file
  session->TcpConnection->write("\r\n");

  //Send the file
  if(session->Response.DataType == daf::Http::STRING)
  {
    session->TcpConnection->write(session->Response.string.c_str());
  }
  else
  {
    //Check the file size
    fseek(session->Response.file, 0, SEEK_END);
    size_t len = ftell(session->Response.file);
    fseek(session->Response.file, 0, SEEK_SET);

    //Capture the fileno so we can try to avoid userspace transfer using sendfile()
    int ffd = fileno(session->Response.file);

    if(ffd < 0)
    {
      throw HttpException("Could not get file descriptor of file to be sent!", ffd, -500);
    }

    //Use sendfile if it is available
#ifndef _WIN32
    ssize_t ret = sendfile(session->TcpConnection->getFileDescriptor(), ffd, 0, len);
#else
    ssize_t ret = 0;
#endif

    //Check to make sure that the transfer was successful
    if(ret < 0)
    {
      throw HttpException("Could not complete transfer!", (int) ret, -500);
    }
    else if((size_t) ret < len)
    {
      //Write the rest of the bytes regularly because sendfile failed midway
      //Note that sendfile can only support files up to 2 GiB
      char buffer[16384];
      size_t nreps = len / 16384;
      size_t carry = len % 16384;

      for(size_t i = 0; i < nreps; i++)
      {
        if(fread(buffer, 16384, 1, session->Response.file) > 0)
          session->TcpConnection->write(buffer, 16384);
      }

      if(fread(buffer, carry, 1, session->Response.file) > 0)
        session->TcpConnection->write(buffer, carry);
    }

    //Close the connection with a newline
    session->TcpConnection->write("\r\n\r\n");
  }
}

//Handles basic requests
void daf::HttpServer::Server::request_handler(daf::HttpServer::Session& session)
{
  session.Response.string = "<!DOCTYPE html><html><body><h1>Hello World!</h1></body></html>";
}

//Handles websocket requests
void daf::HttpServer::Server::websocket_handler(daf::HttpServer::Socket& socket)
{
}



//Generate exception backtrace
void daf::HttpServer::Exception::generateStacktrace()
{
#ifndef _WIN32
  void* traces[25];
  backtrace_size = backtrace(traces, 25);
  backtrace_strings = backtrace_symbols(traces, backtrace_size);
#else
  backtrace_strings = NULL;
#endif
}

//Return the previously-generated backtrace
String daf::HttpServer::Exception::getStacktrace()
{
  std::stringstream str;
  for(int i = 0; i < backtrace_size; i++)
  {
    str << "[" << i << "]: " << backtrace_strings[i] << std::endl;
  }
  return str.str();
}

//GC for the backtrace
daf::HttpServer::Exception::~Exception()
{
  if(backtrace_strings != NULL)
    free(backtrace_strings);
}
