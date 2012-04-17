
#include "FormulaUU.h"

#include "Interpolator.h"

// see email Gabriel Kielland, 2012-02-24 10:57
namespace {

const float C10 = 6.11213, C20 = 17.5043, C30 = 241.2;

}

/** Calculate dew point from temperature TA and relative humidity UU. */
float formulaTD(float TA, float UU)
{
    if( UU <= 0 || UU >= 100 || TA == ::Interpolator::INVALID )
        return ::Interpolator::INVALID;
    float C1, C2, C3;
    if( TA >= 0 ) {
        C1 = C10;
        C2 = C20;
        C3 = C30;
    } else {
        C1 = 6.11153;
        C2 = 22.4433;
        C3 = 272.186;
    }
    const float lu = std::log(UU/100.0);
    const float a = (C20*TA)/(C30+TA);
    return C3*(a + lu)/(C2-a-lu);
}

// ------------------------------------------------------------------------

/** Calculate relative humidity from temperature TA and dew point TD. */
float formulaUU(float TA, float TD)
{
    if( TD == ::Interpolator::INVALID || TA == ::Interpolator::INVALID )
        return ::Interpolator::INVALID;
    return 100*std::exp( (C20*TD)/(C30+TD) - (C20*TA)/(C30+TA) );
}