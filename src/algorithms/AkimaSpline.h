// -*- c++ -*-

#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <math.h>
#include <vector>
//GNU Statistical library
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

using namespace std;

class AkimaSpline{

public:
    AkimaSpline(const std::vector<double>& xt, const std::vector<double>& yt);
    ~AkimaSpline();

    double AkimaPoint(double y);
    //int AkimaPoints();

private:
    gsl_interp_accel *acc;
    gsl_spline *spline;
};
