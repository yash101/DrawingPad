/*
 * Server.cpp
 *
 *  Created on: Dec 6, 2015
 *      Author: yash
 */

#include "Server.h"
#include "main.h"
#include "DAF/entry.h"

void read_configuration()
{
  if(Configuration()["Http.TimeoutMicroseconds"].size() == 0)
  {
    Configuration()["Http.TimeoutMicroseconds"] = "0";
    Configuration().flush();
  }

  if(Configuration()["Http.TimeoutSeconds"].size() == 0)
  {
    Configuration()["Http.TimeoutSeconds"] = "180";
    Configuration().flush();
  }

  if(Configuration()["Http.MaximumConcurrentConnections"].size() == 0)
  {
    Configuration()["Http.MaximumConcurrentConnections"] = "0";
    Configuration().flush();
  }

  if(Configuration()["Http.MaximumTcpQueueSize"].size() == 0)
  {
    Configuration()["Http.MaximumTcpQueueSize"] = "5";
    Configuration().flush();
  }

  if(Configuration()["Http.Port"].size() == 0)
  {
    Configuration()["Http.Port"] = "1234";
    Configuration().flush();
  }

  daf::pollMime("htm", "text/html");
  daf::pollMime("html", "text/html");
  daf::pollMime("css", "text/css");
  daf::pollMime("js", "text/javascript");
  daf::pollMime("png", "image/png");
  daf::pollMime("jpg", "image/jpg");
  daf::pollMime("gif", "image/gif");
}

void set_http_handlers()
{
  server.defaultHostname().setStatic("(.*)", "Resources.Http");
  server.defaultHostname().setStatic("/whiteboard", "Resources.Http/whiteboard.html");
}
