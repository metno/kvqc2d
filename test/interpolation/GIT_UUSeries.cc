
#include "GapInterpolationTestBase.hh"

TEST_F(GapInterpolationTest, UUSeries)
{
    DataList data(99927, 211, 330);
    data.add("2012-05-02 11:00:00",      -9.1,      -9.1, "0111000000100010", "")
        .add("2012-05-02 12:00:00",      -9.0,      -9.0, "0111000000100010", "")
        .add("2012-05-02 13:00:00",      -9.0,      -9.0, "0111000000100010", "")
        .add("2012-05-02 14:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2012-05-02 15:00:00",      -8.6,      -8.6, "0110000000100010", "")
        .add("2012-05-02 16:00:00",      -8.1,      -8.1, "0111000000100010", "")
        .add("2012-05-02 17:00:00",      -7.9,      -7.9, "0111000000100010", "");
    data.setParam(262);
    data.setStation(99927).setType(330)
        .add("2012-05-02 11:00:00",      79.0,      79.0, "0101000000000010", "")
        .add("2012-05-02 12:00:00",      79.0,      79.0, "0101000000000010", "")
        .add("2012-05-02 13:00:00",      76.0,      76.0, "0101000000000010", "")
        .add("2012-05-02 14:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2012-05-02 15:00:00",      78.0,      78.0, "0100000000000010", "")
        .add("2012-05-02 16:00:00",      78.0,      78.0, "0101000000000010", "")
        .add("2012-05-02 17:00:00",      83.0,      83.0, "0101000000000010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =    5\n"
           << "Start_DD   =    2\n"
           << "Start_hh   =   11\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =    5\n"
           << "End_DD     =    2\n"
           << "End_hh     =   18\n"
           << "TypeId     =  330\n"
           << "Parameter  = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=80,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  = par=262,minPar=264,maxPar=265,minVal=0,maxVal=100,offsetCorrectionLimit=5,fluctuationLevel=2\n";
    AlgorithmConfig params;
    ASSERT_PARSE_CONFIG(params, config);
    ASSERT_CONFIGURE(algo, params);

    ASSERT_RUN(algo, bc, 2);
    EXPECT_NEAR(-8.8, bc->update(0).corrected(), 0.1);
    EXPECT_NEAR(77, bc->update(1).corrected(), 0.3);

    ASSERT_RUN(algo, bc, 0);
}
