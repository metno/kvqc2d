
#ifndef DEBUG_H
#define DEBUG_H 1

#include <milog/milog.h>

#ifndef NDEBUG
#define DBG(x) do { LOGDEBUG(x); std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl; } while(false);
#define INF(x) do { LOGINFO(x);  std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl; } while(false);
#else
#define DBG(x) do { /* nothing */ } while(false);
#define INF(x) do { LOGINFO(x); } while(false);
#endif
#define DBGV(x) DBG( #x "='" << x << "'" )

#endif /* DEBUG_H */
