#ifndef DEFINITIONS_HXX
#define DEFINITIONS_HXX
#define DEBUG               true       //Whether to enable the rather long debug messages!
#define MUTEX_TIMEOUT       100         //Hmmmm... Stuck threads bothering you? Set this timeout wisely and then
                                        //the mutex will expire after X number of milliseconds :)
#define DISABLE_FILECACHE               //Let's disable the cache. Only for development purposes. Will impact performance.
                                        //Comment out DISABLE_FILECACHE to disable!
#define SESSION_TIMEOUT     60000       //How long a session should last before deletion. 60 seconds is good.
                                        //Note that we use miliseconds!
#define PORTNO_PAINTSERVER  1234        //What port to use for the main server
#define PORTNO_TELNET       2345        //What port to use for the telnet server
//#define FORK                true        //Whether to fork the process and daemonize!
#define TERMINAL            true        //Whether to launch terminal
#define SIGHANDLE           true        //Whether to handle signals
#define AUTOKILL            true        //Whether to kill old children or let them live! Sounds cannibalistic, right? :)

//#define DEVELOPMENT_DEFAULTS
#define DEPLOYMENT_DEFAULTS

//====================[VALIDATION]=====================
//Some configuration settings.
//So you get some human-readable errors and warnings instead of random faults
//For the errors shown here, please use the error message, not the line number!
#if defined(FORK)
#ifdef __WIN32
#pragma message("You are in Windows! Cannot fork! Forking disabled!")
#undef FORK
#endif
#endif
#if defined(DEVELOPMENT_DEFAULTS) && defined(DEPLOYMENT_DEFAULTS)
#error Only uncomment either Development Defaults, Deployment Defauluts or none!
#endif

#ifdef DEVELOPMENT_DEFAULTS
#undef DEBUG
#undef DISABLE_CACHE
#undef FORK
#undef TERMINAL
#undef SIGHANDLE
#pragma message("Development Build Active! Please disable once development is complete!")
#define TERMINAL            true
#define DEBUG               true
#define DISABLE_CACHE       true
#define SIGHANDLE           true
#endif

#ifdef DEPLOYMENT_DEFAULTS
#undef DEBUG
#undef DISABLE_CACHE
#undef FORK
#undef TERMINAL
#undef SIGHANDLE
#define TERMINAL            false
#define DEBUG               false
#define DISABLE_CACHE       false
#define FORK                true
#define SIGHANDLE           true
#endif

#if PORTNO_PAINTSERVER <= 0
#error Invalid port number for the PaintServer!"
#endif
#if SESSION_TIMEOUT < 4000
#pragma message("Timeout value is low. 4000 milliseconds is the keepalive interval. Please consider that sessions may time out when unintended!");
#endif

#ifndef DEBUG
#define DEBUG               true
#endif

#if !FORK
#undef FORK
#endif

#endif // DEFINITIONS_HXX
