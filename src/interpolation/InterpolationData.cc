
#include "InterpolationData.h"

#include <ostream>

float Interpolation::SupportData::value() const
{
    return mValue;
}

float Interpolation::SeriesData::value() const
{
    if (mQuality == DISCARDED)
        return Interpolation::MISSING_VALUE;
    else
        return mValue;
}

bool Interpolation::SeriesData::needsInterpolation() const
{
    return mQuality == BAD
        or mQuality == FAILED
        or mQuality == DISCARDED
        or mQuality == MISSING;
}

std::ostream& operator<<(std::ostream& out, const Interpolation::SupportData& s)
{
    out << "[Supp: v=" << s.value()
        << " u=" << (s.usable() ? 'y' : 'n') << "]";
    return out;
}

std::ostream& operator<<(std::ostream& out, const Interpolation::SeriesData& s)
{
    out << "[Seri: v=" << s.value()
        << " u=" << (s.usable() ? 'y' : 'n')
        << " n=" << (s.needsInterpolation() ? 'Y' : 'N')
        << " q=" << s.quality() << "]";
    return out;
}
