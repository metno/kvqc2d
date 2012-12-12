
#include "GapInterpolationTestBase.hh"

TEST_F(GapInterpolationTest, TAInterpolatedSeries)
{
    DataList data(18700, 211, 330);
    data.add("2012-11-05 08:00:00",       4.5,      "0111000000100010", "")
        .add("2012-11-05 09:00:00",       4.5,      "0111000000100010", "")
        .add("2012-11-05 10:00:00",  -32767.0, 3.5, "0000001100000000", "QC2d-2-I")
        .add("2012-11-05 11:00:00",  -32767.0,      "0000003000000000", "")
        .add("2012-11-05 12:00:00",  -32767.0, 3.5, "0000001100000000", "QC2d-2-I")
        .add("2012-11-05 13:00:00",       4.5,      "0111000000100010", "")
        .add("2012-11-05 14:00:00",       4.5,      "0111000000100010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   11\n"
           << "Start_DD   =   05\n"
           << "Start_hh   =   08\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   11\n"
           << "End_DD     =   05\n"
           << "End_hh     =   14\n"
           << "TypeId     =  330\n"
           << "Parameter  = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=80,offsetCorrectionLimit=15,fluctuationLevel=0.2\n";
    AlgorithmConfig params;
    ASSERT_PARSE_CONFIG(params, config);
    ASSERT_CONFIGURE(algo, params);

    // interpolation yields 4.5 as values before and after the gap are
    // interpolated themselves so they are not used as support points
    ASSERT_RUN(algo, bc, 1);
    EXPECT_NEAR(4.5, bc->update(0).corrected(), 0.01);

    ASSERT_RUN(algo, bc, 0);
}
