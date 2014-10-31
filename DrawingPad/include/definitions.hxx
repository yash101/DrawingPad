#ifndef DEFINITIONS_HXX
#define DEFINITIONS_HXX
#define DEBUG true                      //Whether to enable the rather long debug messages!
#define MUTEX_TIMEOUT   100             //Hmmmm... Stuck threads bothering you? Set this timeout wisely and then
                                        //the mutex will expire after X number of milliseconds :)
#define DISABLE_FILECACHE   true            //Let's disable the cache. Only for development purposes. Will impact performance.
#endif // DEFINITIONS_HXX
