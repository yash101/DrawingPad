/*
 * AutoMutex.h
 *
 *  Created on: Nov 8, 2015
 *      Author: yash
 */

#ifndef AUTOMUTEX_H_
#define AUTOMUTEX_H_

#include <stddef.h>

//Namespace for misc. tools
namespace tool
{
  //This class allows us to play with mutexes
  template<typename MutexClassType>
  class AutoMutex
  {
  private:
    //This holds a pointer to any type of mutex object
    MutexClassType* mutex;

  public:
    //This is a default initializer.
    AutoMutex() :
      mutex(NULL)
    {}

    //This takes a reference
    AutoMutex(MutexClassType& mtx) :
      mutex(&mtx)
    {
      if(mutex != NULL)
        mutex->lock();
    }

    //This takes a pointer
    AutoMutex(MutexClassType* mtx) :
      mutex(mtx)
    {
      if(mutex != NULL)
      {
        mutex->lock();
      }
    }

    //This unlocks the mutex
    virtual ~AutoMutex()
    {
      if(mutex != NULL)
      {
        mutex->unlock();
      }
    }
  };
}

#endif /* AUTOMUTEX_H_ */
