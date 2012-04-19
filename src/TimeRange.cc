
#include "TimeRange.h"

#define NDEBUG 1
#include "debug.h"

int TimeRange::days() const
{
    return 24*hours();
}

// ------------------------------------------------------------------------

int TimeRange::hours() const
{
    return miutil::miTime::hourDiff(t1, t0);
}
