
#ifndef DEBUG_H
#define DEBUG_H 1

#include <milog/milog.h>

#ifndef NDEBUG
#define DBGINF(x) std::cout << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]\n    " << x << std::endl
#define DBG(x) do { LOGDEBUG(x); DBGINF(x); } while(false);
#define INF(x) do { LOGINFO(x);  DBGINF(x); } while(false);
#else
#define DBG(x) do { /* nothing */ } while(false);
#define INF(x) do { LOGINFO(x); } while(false);
#endif
#define DBG1(x) #x "='" << x << "' "
#define DBGV(x) DBG( DBG1(x) )

#endif /* DEBUG_H */
