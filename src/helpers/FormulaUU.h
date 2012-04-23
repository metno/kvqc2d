// -*- c++ -*-

#ifndef FORMULA_UU_h
#define FORMULA_UU_h 1

/** Calculate dew point from temperature TA and relative humidity UU. */
float formulaTD(float TA, float UU);

/** Calculate relative humidity from temperature TA and dew point TD. */
float formulaUU(float TA, float TD);

extern const int UU_INVALID;

#endif /* FORMULA_UU_h */
