// -*- c++ -*-

#ifndef AkimaSplineTest_H
#define AkimaSplineTest_H 1

#include "algorithms/AkimaSpline.h"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class AkimaSplineTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(AkimaSplineTest);
    CPPUNIT_TEST(testStraightLine);
    CPPUNIT_TEST(testParabola);
    CPPUNIT_TEST_SUITE_END();

public:
    void testStraightLine();
    void testParabola();
};

#endif // AkimaSplineTest_H
