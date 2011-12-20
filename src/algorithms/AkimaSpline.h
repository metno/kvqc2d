// -*- c++ -*-

#ifndef AkimaSpline_H
#define AkimaSpline_H 1

#include <vector>
//GNU Statistical library
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

using namespace std;

// TODO maybe use template+iterators instead of std::vector
class AkimaSpline{

public:
    AkimaSpline(const std::vector<double>& xt, const std::vector<double>& yt);
    ~AkimaSpline();

    double operator() (double x) const
        { return AkimaPoint(x); }

    double AkimaPoint(double x) const;
    //int AkimaPoints();

private:
    gsl_interp_accel *acc;
    gsl_spline *spline;
};

#endif
