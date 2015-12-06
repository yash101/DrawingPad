/*
 * main.cpp
 *
 *  Created on: Dec 6, 2015
 *      Author: yash
 */

#include <string.h>

#include "main.h"
#include "DAF/entry.h"
#include "Server.h"

int main(int argc, char** argv)
{
  init(argc, argv);
  read_configuration();

  set_http_handlers();

  server.setSocketTimeout(
    atoi(Configuration()["Http.TimeoutMicroseconds"].c_str()),
    atoi(Configuration()["Http.TimeoutSeconds"].c_str())
  );
  server.setMaxConnectedClients(atoi(Configuration()["Http.MaximumConcurrentConnections"].c_str()));
  server.setTcpQueueSize(atoi(Configuration()["Http.MaximumTcpQueueSize"].c_str()));
  server.setPort(atoi(Configuration()["Http.Port"].c_str()));
  server.startServer();
}

daf::ApplicationServer server;
