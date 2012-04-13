
#include "Akima.h"

#include <algorithm>
#include <cmath>

const double Akima::INVALID = -99999999;

// ------------------------------------------------------------------------

double Akima::interpolate(double x) const
{
    const int N = (int)mX.size()-1;
    if( N<4 || x < mX.front() )
        return -INVALID;
    std::vector<double>::const_iterator iX1 = std::lower_bound(mX.begin(), mX.end(), x);
    if( iX1 == mX.end() )
        return -INVALID;
    const int i = std::max(0, static_cast<int>(iX1 - mX.begin() - 1));
    if( i >= N )
        return -INVALID;
    if( x < mX[i] || x > mX[i+1] ) {
        //std::cout << "x=" << x << " i=" << i << " x[i]=" << mX[i] << " x[i+1]=" << mX[i+1] << std::endl;
        return -INVALID;
    }

    double mmm[5];
    for(int j=std::max(i-2, 0); j<std::min(i+3, N); ++j)
        mmm[j-i+2] = (mY[j+1]-mY[j])/(mX[j+1]-mX[j]);
    if( i < 2 ) {
        const double m2 = mmm[2-i], m3 = mmm[3-i];
        mmm[1-i] = 2*m2 - m3;
        if( i == 0 )
            mmm[0] = 3*m2 - 2*m3;
    }
    if( i > N-3 ) {
        const double m1 = mmm[N+1-i], m0 = mmm[N-i];
        mmm[N+2-i] = 2*m1 - m0;
        if( i > N-2 )
            mmm[N+3-i] = 3*m1 - 2*m0;
    }

    const double a0  = fabs(mmm[1] - mmm[0]), NE0 = a0 + fabs(mmm[3] - mmm[2]);
    const double tR0 = (NE0>0) ? (mmm[1] + a0*(mmm[2] - mmm[1])/NE0) : mmm[2];
    const double a1  = fabs(mmm[2] - mmm[1]), NE1 = a1 + fabs(mmm[4] - mmm[3]);
    const double tL1 = (NE1>0) ? (mmm[2] + a1*(mmm[3] - mmm[2])/NE1) : mmm[2];

    const double h = mX[i+1] - mX[i];
    const double a = mY[i];
    const double b = tR0;
    const double c = (3*mmm[2] - 2*tR0 - tL1)/h;
    const double d = (tR0 + tL1 - 2*mmm[2])/(h*h);
    const double x0 = x - mX[i];
    return a + x0*(b + x0*(c + x0*d));
}
