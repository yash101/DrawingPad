#ifndef DEFINITIONS_HXX
#define DEFINITIONS_HXX
#define DEBUG               false       //Whether to enable the rather long debug messages!
#define MUTEX_TIMEOUT       100         //Hmmmm... Stuck threads bothering you? Set this timeout wisely and then
                                        //the mutex will expire after X number of milliseconds :)
#define DISABLE_FILECACHE               //Let's disable the cache. Only for development purposes. Will impact performance.
                                        //Comment out DISABLE_FILECACHE to disable!
#define SESSION_TIMEOUT     10000       //How long a session should last before deletion
#define PORTNO_PAINTSERVER  1234        //What port to use for the main server
#define PORTNO_TELNET       2345        //What port to use for the telnet server
#endif // DEFINITIONS_HXX
