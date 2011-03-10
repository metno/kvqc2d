
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

private:

public:
 double tt[100], pp[100]; 
 int npoints;

 AkimaSpline(std::vector<double> xt, std::vector<double> yt);

 double AkimaPoint(double y);
 int AkimaPoints();

 ~AkimaSpline(){};

};

