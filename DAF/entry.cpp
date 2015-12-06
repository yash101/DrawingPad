/*
 * entry.cpp
 *
 *  Created on: Nov 7, 2015
 *      Author: yash
 */

#ifndef CONFIGURATION_FILE_LOCATION
#define CONFIGURATION_FILE_LOCATION "Config.cfg"
#endif

#include "entry.h"
#include <stdio.h>
static daf::Config* conf = NULL;

//This function should be called as early in the program as possible
//It parses arguments and sets up the configuration system
int init(int argc, char** argv)
{
  Arguments::count = argc;
  Arguments::arguments = argv;
  conf = new daf::Config(CONFIGURATION_FILE_LOCATION);
  if(conf == NULL)
  {
    fprintf(stderr, "Unable to allocate memory for daf::Config!\n");
    return -1;
  }

  return 0;
}

//Gives access to the internal static configuration object
daf::Config& Configuration()
{
  return (*conf);
}

//Signal handlers for dynamic features
//Currently implemented:
//  - SIGUSR1 reloads the configuration file immediately

#include <signal.h>
#include <unistd.h>

#ifndef CONF_RELOAD_SIGNAL
#define CONF_RELOAD_SIGNAL SIGUSR1
#endif

static void sig_reload(int sig)
{
  if(sig == CONF_RELOAD_SIGNAL)
  {
    Configuration().refresh();
  }
}

bool set_signal_handlers()
{
  signal(CONF_RELOAD_SIGNAL, sig_reload);
  return true;
}

// Arguments are stored in the namespace, Arguments
// Defined in entry.h
int Arguments::count;
char** Arguments::arguments;
