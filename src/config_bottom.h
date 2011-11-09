
#ifndef KVQC2D_CONFIG_H
#error "config_bottom.h must be included via config.h"
#endif

#include "sqlite3.h"
#if SQLITE_VERSION_NUMBER >= 3006022
#define HAVE_SQL_WITH_WORKING_SUBSTR_IN 1
#endif
