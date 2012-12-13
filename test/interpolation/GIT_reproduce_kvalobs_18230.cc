
#include "GapInterpolationTestBase.hh"

#define FWD_DECL_ONLY
#include "data_reproduce_kvalobs_18230.cc"

TEST_F(GapInterpolationTest, reproduce_kvalobs_18230)
{
    data_reproduce_kvalobs_18230(db);

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   12\n"
           << "Start_DD   =   11\n"
           << "Start_hh   =   13\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   12\n"
           << "End_DD     =   12\n"
           << "End_hh     =   13\n"
           << "TypeId     =  789\n"
           << "Parameter  = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=100,offsetCorrectionLimit=15,fluctuationLevel=0\n";
    AlgorithmConfig params;
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 16);
   
    EXPECT_EQ(miutil::miTime("2012-12-11 18:00:00"), bc->update(0).obstime());
    EXPECT_NEAR(-11.9, bc->update(0).corrected(), 0.05);

    EXPECT_EQ(miutil::miTime("2012-12-12 09:00:00"), bc->update(15).obstime());
    EXPECT_NEAR(-12.0, bc->update(15).corrected(), 0.05);
    
    for (int i=1; i<15; ++i) {
        EXPECT_EQ(-32767, bc->update(i).corrected());
        EXPECT_EQ("0000003300000000", bc->update(i).controlinfo().flagstring());
    }

    ASSERT_RUN(algo, bc, 0);
}
