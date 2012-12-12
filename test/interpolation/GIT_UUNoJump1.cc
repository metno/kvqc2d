
#include "GapInterpolationTestBase.hh"

TEST_F(GapInterpolationTest, UUNoJump1)
{
    DataList data(90720, 211, 342);
    data.add("2012-04-26 17:00:00",       8.6, "0111000000100010", "")
        .add("2012-04-26 18:00:00",       8.2, "0111000000100010", "")
        .add("2012-04-26 19:00:00",       9.1, "0111000000100010", "")
        .add("2012-04-26 20:00:00",       8.2, "0111000000100010", "")
        .add("2012-04-26 21:00:00",       8.6, "0111000000100010", "")
        .add("2012-04-26 22:00:00",       7.3, "0111000000100010", "")
        .add("2012-04-26 23:00:00",       7.7, "0111000000100010", "");
    data.setParam(262);
    data.setStation(90720).setType(342)
        .add("2012-04-26 17:00:00",      89.0, "0101000000000010", "")
        .add("2012-04-26 18:00:00",      89.8, "0101000000000010", "")
        .add("2012-04-26 19:00:00",      90.2, "0101000000000010", "")
        .add("2012-04-26 20:00:00",  -32767.0, "0000003000000000", "")
        .add("2012-04-26 21:00:00",      92.4, "0100000000000010", "")
        .add("2012-04-26 22:00:00",      93.0, "0101000000000010", "")
        .add("2012-04-26 23:00:00",      93.0, "0101000000000010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   04\n"
           << "Start_DD   =   25\n"
           << "Start_hh   =   17\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   04\n"
           << "End_DD     =   26\n"
           << "End_hh     =   23\n"
           << "TypeId     =  342\n"
           << "Parameter  = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=80,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  = par=262,minPar=264,maxPar=265,minVal=0,maxVal=100,offsetCorrectionLimit=5,fluctuationLevel=2\n";
    AlgorithmConfig params;
    ASSERT_PARSE_CONFIG(params, config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1);
    // although ~97 looks strange to the eye, it seems to be "correct" as there is
    // a drop in TA at the same time
    EXPECT_NEAR(97, bc->update(0).corrected(), 0.3);
    ASSERT_RUN(algo, bc, 0);
}
