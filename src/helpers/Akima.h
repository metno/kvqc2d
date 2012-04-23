// -*- c++-*-

#ifndef AKIMA_H
#define AKIMA_H 1

#include <vector>

class Akima {
public:
    Akima& add(double x, double y)
        { mX.push_back(x); mY.push_back(y); return *this; }

    void clear()
        { mX.clear(); mY.clear(); }

    int count() const
        { return mX.size(); }

    double interpolate(double x) const;

    double distance(double x) const;

    static const double INVALID;

private:
    int findIndex(double x) const;

private:
    std::vector<double> mX, mY;
};

#endif // AKIMA_H
