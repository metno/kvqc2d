
#include "GapInterpolationTestBase.hh"

#define FWD_DECL_ONLY
#include "data_reproduce_kvalobs_68290.cc"

TEST_F(GapInterpolationTest, reproduce_kvalobs_68290)
{
    data_reproduce_kvalobs_68290(db);

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   12\n"
           << "Start_DD   =   31\n"
           << "Start_hh   =   17\n"
           << "End_YYYY   = 2013\n"
           << "End_MM     =   01\n"
           << "End_DD     =   01\n"
           << "End_hh     =   05\n"
           << "TypeId     = -32767\n"
           << "Parameter  = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=100,offsetCorrectionLimit=15,fluctuationLevel=0\n";
    AlgorithmConfig params;
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 9);
    for (int i=0; i<9; ++i)
        EXPECT_LT(-10, bc->update(i).corrected());
    ASSERT_RUN(algo, bc, 0);
}
