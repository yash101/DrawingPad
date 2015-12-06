/*
 * entry.h
 *
 *  Created on: Nov 7, 2015
 *      Author: yash
 */

#ifndef ENTRY_H_
#define ENTRY_H_
#include "Configuration.h"

//*** Call this function as early as possible!
int init(int argc, char** argv);

//Call this function if you want to be able to use the dynamic loading features
bool set_signal_handlers();

//Where the Arguments are stored for public use
namespace Arguments
{
  extern int count;
  extern char** arguments;
}

//Retrieves the global configuration file
daf::Config& Configuration();

#endif /* ENTRY_H_ */
